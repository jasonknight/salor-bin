
#include "display_link.h"
#include <stdio.h>
#include <string.h>
#include "libdlo.h"
#include "dlo_defs.h"
#include <QDebug>
/** Report an error and exit.
 *
 *  @param  str  Pointer to the error message string.
 */
static void my_error(const char * const str)
{
  printf("test: ERROR: %s\n", str);
  exit(1);
}


/** Load a Windows BMP file into memory.
 *
 *  @param  bmpfile  Pointer to the filename.
 *
 *  @return  Pointer to the loaded bitmap's structure (or NULL if failed).
 *
 *  Load a bitmap from a file and create a @a dlo_fbuf_t structure to suit.
 *  If the bitmap has a palette, we need to swap the red/blue order of the
 *  colour components in order to convert the palette entries into
 *  @a dlo_col32_t values.
 */
static bmp_t *load_bmp(const char * const bmpfile)
{
  long int      size;
  bmp_t        *bmp;
  FILE         *fp;
  char         filename[255];

  strncat(filename, bmpfile, sizeof(filename) - 8);
  fp = fopen(filename, "rb");
  if (!fp) {
    strncat(filename, bmpfile, sizeof(filename) - 14);
    fp = fopen(filename, "rb");
    if (!fp) {
      return NULL;
    }
  }

  if (fseek(fp, 0, SEEK_END))
    return NULL;

  size = ftell(fp);
  if (size == -1L)
    return NULL;

  if (fseek(fp, 0, SEEK_SET))
    return NULL;

  bmp = (bmp_t*)malloc(size);
  if (!bmp)
    return NULL;

  if (1 != fread(bmp, size, 1, fp))
    goto error;

  if (bmp->hdr.magic != BMP_MAGIC)
    goto error;

  /* If there is a palette, we need to reverse the RGB component order */
  if (bmp->dib.pal_entries)
  {
    dlo_col32_t *palette;
    uint32_t      i;

    palette = (dlo_col32_t *)((unsigned long)bmp + sizeof(bmp_header_t) + sizeof(dib_header_t));

    for (i = 0; i < bmp->dib.pal_entries; i++)
    {
      dlo_col32_t col = palette[i];

      palette[i] = DLO_RGB(DLO_RGB_GETBLU(col), DLO_RGB_GETGRN(col), DLO_RGB_GETRED(col));
    }
  }
  return bmp;

error:
  free(bmp);

  return NULL;
}

/** Given a bitmap pointer, check various aspects of it and return a framebuffer structure pointer.
 *
 *  @param  bmp  Pointer to a loaded bitmap structure.
 *
 *  @return  Pointer to a framebuffer structure associated with the bitmap.
 *
 *  NOTE: the bitmap plotting code requires some special-case Windows BMP format
 *  bitmaps as input; they must include a DIB header (very common) and their width
 *  must be such that they don't require any padding bytes at the end of each pixel
 *  row.
 *
 *  The padding issue could be fixed very simply by calling the dlo_copy_host_bmp()
 *  for each pixel row individually but for the sake of a simple demo, we impose
 *  the constraint and make only one call to dlo_copy_host_bmp() here.
 */
static dlo_fbuf_t *bmp_to_fbuf(const bmp_t * const bmp)
{
  static dlo_fbuf_t fbuf;

  printf("bmp->hdr.magic       %04X\n"
         "bmp->hdr.file_sz     %08X (%u)\n"
         "bmp->hdr.reserved1   %04X\n"
         "bmp->hdr.reserved2   %04X\n"
         "bmp->hdr.pix_offset  %08X\n"
         "bmp->dib.dib_hdr_sz  %08X\n"
         "bmp->dib.width       %08X (%u)\n"
         "bmp->dib.height      %08X (%u)\n"
         "bmp->dib.col_planes  %04X\n"
         "bmp->dib.bpp         %04X (%u)\n"
         "bmp->dib.compression %08X\n"
         "bmp->dib.raw_size    %08X (%u)\n"
         "bmp->dib.x_pix_meter %08X\n"
         "bmp->dib.y_pix_meter %08X\n"
         "bmp->dib.pal_entries %08X (%u)\n"
         "bmp->dib.imp_cols    %08X\n",
         bmp->hdr.magic,
         bmp->hdr.file_sz, bmp->hdr.file_sz,
         bmp->hdr.reserved1,
         bmp->hdr.reserved2,
         bmp->hdr.pix_offset,
         bmp->dib.dib_hdr_sz,
         bmp->dib.width, bmp->dib.width,
         bmp->dib.height, bmp->dib.height,
         bmp->dib.col_planes,
         bmp->dib.bpp, bmp->dib.bpp,
         bmp->dib.compression,
         bmp->dib.raw_size, bmp->dib.raw_size,
         bmp->dib.x_pix_meter,
         bmp->dib.y_pix_meter,
         bmp->dib.pal_entries, bmp->dib.pal_entries,
         bmp->dib.imp_cols);

  if (bmp->dib.compression)
    my_error("Unsupported bitmap compression mode");
  if (bmp->dib.col_planes != 1)
    my_error("Unsupported bitmap colour plane specification");
  if ((bmp->dib.width * BYTES_PER_PIXEL(bmp->dib.bpp)) & 3)
    my_error("Bitmap width must be whole multiple of four bytes (no padding)");

  fbuf.width  = bmp->dib.width;
  fbuf.height = bmp->dib.height;
  fbuf.base   = bmp->hdr.pix_offset + (uint8_t *)bmp;
  fbuf.stride = fbuf.width;
  switch (bmp->dib.bpp)
  {
    case 8:
    {
          // Jason -  The compiler seems to want sizeof(bmp_header_t) to be dereferenced...
      fbuf.fmt = (dlo_pixfmt_t)*(sizeof(bmp_header_t) + sizeof(dib_header_t) + (uint8_t *)bmp);
      if (bmp->dib.pal_entries != 256)
        my_error("Unsupported bitmap palette size");
      break;
    }
    case 16:
    {
      fbuf.fmt = dlo_pixfmt_srgb1555;
      break;
    }
    case 24:
    {
      fbuf.fmt = dlo_pixfmt_rgb888;
      break;
    }
    case 32:
    {
      fbuf.fmt = dlo_pixfmt_argb8888;
      break;
    }
    default:
      my_error("Unsupported bitmap colour depth");
  }
  return &fbuf;
}



int display_link_write_image(char const * bmpfile){
qDebug() << "display_link_write called..." << bmpfile;
dlo_init_t ini_flags = { 0 };
dlo_final_t fin_flags = { 0 };
dlo_claim_t cnf_flags = { 0 };
dlo_retcode_t err;
dlo_dev_t uid = 0;

/* Initialise libdlo */
ERR_GOTO(dlo_init(ini_flags));

/* Look for a DisplayLink device to connect to */
uid = dlo_claim_first_device(cnf_flags, 0);
if (uid) {
    /* we claimed a device */



    dlo_mode_t desc;
    /* Select a mode */
    desc.view.base = 0; /* Base address in device memory for this screen display */
    desc.view.width = 800;
    desc.view.height = 480; /* We can use zero as a wildcard here */
    desc.view.bpp = 24; /* Can be a wildcard, meaning we don't mind what colour depth */
    desc.refresh = 0; /* Refresh rate in Hz. Can be a wildcard; any refresh rate */
    ERR(dlo_set_mode(uid, &desc));



    /* =========== PAINT BITMAP ======================= */
      bmp_t          *bmp;
      dlo_fbuf_t    *fbuf;
      dlo_dot_t      dot;
      dlo_bmpflags_t flags = { 0 };

      /* Load the Windows BMP bitmap file into memory */
      qDebug() << "Loading: " << bmpfile;
      bmp = load_bmp(bmpfile);
      NERR(bmp);

      /* Initialise a dlo_fbuf structure from our loaded bitmap file  */
      fbuf = bmp_to_fbuf(bmp);
      NERR_GOTO(fbuf);

      dot.x = 0;
      dot.y = 0;
      flags.v_flip = 1;
      ERR_GOTO(dlo_copy_host_bmp(uid, flags, fbuf, NULL, &dot));

      free(bmp);



    /* Release the device when we're finished with it */
    ERR_GOTO(dlo_release_device(uid));
} else {
    qDebug() << "display_link_write_image: No Device Found";
}
/* Finalise libdlo */
ERR_GOTO(dlo_final(fin_flags));
return 0;
error:
/* The ERR_GOTO() macro jumps here if there was an error */
qDebug() << "Error: " << dlo_strerror(err);
printf("Error %u '%s'\n", (int)err, dlo_strerror(err));
return 1;
}

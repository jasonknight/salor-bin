#ifndef DISPLAY_LINK_H
#define DISPLAY_LINK_H
#include <stdio.h>
#include <string.h>
#include "libdlo.h"
#include "dlo_defs.h"

/** Windows BMP magic ID bytes.
 */
#define BMP_MAGIC (0x4D42)


/** Given a bits per pixel value, return the bytes per pixel.
 */
#define BYTES_PER_PIXEL(bpp) (unsigned int)(((bpp) + 7) / 8)


/** The main header block from a Windows BMP file.
 */
typedef struct __packed bmp_header_s
{
  uint16_t magic;          /**< Magic ID bytes for Windows BMP format. */
  uint32_t file_sz;        /**< Total bitmap file size (bytes). */
  uint16_t reserved1;      /**< Unused. */
  uint16_t reserved2;      /**< Unused. */
  uint32_t pix_offset;     /**< Offset from start of file to start of pixel data (bytes). */
} bmp_header_t;            /**< A struct @a bmp_header_s. */


/** The DIB header block from a Windows BMP file.
 */
typedef struct __packed dib_header_s
{
  uint32_t dib_hdr_sz;     /**< Size of the DIB header block (bytes). */
  uint32_t width;          /**< Width of the bitmap (pixels). */
  uint32_t height;         /**< Height of the bitmap (pixels). */
  uint16_t col_planes;     /**< Number of colour planes. */
  uint16_t bpp;            /**< Bits per pixel. */
  uint32_t compression;    /**< Compression, pixel format. */
  uint32_t raw_size;       /**< Size of the raw pixel data. */
  uint32_t x_pix_meter;    /**< Horizontal resolution (pixels per meter). */
  uint32_t y_pix_meter;    /**< Vertical resolution (pixels per meter). */
  uint32_t pal_entries;    /**< Number of palette entries. */
  uint32_t imp_cols;       /**< Important colours (ignored). */
} dib_header_t;            /**< A struct @a dib_header_s. */


/** A Windows BMP file.
 */
typedef struct __packed bmp_s
{
  bmp_header_t hdr;        /**< Windows BMP header block. */
  dib_header_t dib;        /**< DIB header block. */
  uint8_t      data[];     /**< Pixel data. */
} bmp_t;                   /**< A struct @a bmp_s. */

int display_link_write_image(char const * bmpfile);
#endif // DISPLAY_LINK_H

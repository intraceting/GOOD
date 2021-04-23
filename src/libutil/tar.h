/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_TAR_H
#define GOOD_UTIL_TAR_H

#include "general.h"
#include "buffer.h"

/**
 * This is the ustar (Posix 1003.1) header.
 * 
 * 512 Bytes.
 */
typedef struct _good_ustar_hdr
{
    char name[100];         /*   0 Dateiname*/
    char mode[8];           /* 100 Zugriffsrechte*/
    char uid[8];            /* 108 Benutzernummer*/
    char gid[8];            /* 116 Benutzergruppe*/
    char size[12];          /* 124 Dateigroesze*/
    char mtime[12];         /* 136 Zeit d. letzten Aenderung*/
    char chksum[8];         /* 148 Checksumme*/
    char typeflag;          /* 156 Typ der Datei*/
    char linkname[100];     /* 157 Zielname des Links*/
    char magic[TMAGLEN];    /* 257 "ustar"*/
    char version[TVERSLEN]; /* 263 Version v. star*/
    char uname[32];         /* 265 Benutzername	*/
    char gname[32];         /* 297 Gruppenname*/
    char devmajor[8];       /* 329 Major bei Geraeten*/
    char devminor[8];       /* 337 Minor bei Geraeten*/
    char prefix[155];       /* 345 Prefix fuer t_name*/
    char mfill[12];         /* 500 Filler bis 512*/
} good_ustar_hdr;

/*
 * gnu tar extensions:
 */

/** long link magic.*/
#define GOOD_USTAR_LONGNAME_MAGIC "././@LongLink"
/** including NULL byte. */
#define GOOD_USTAR_LONGNAME_MAGIC_LEN 14
/** Identifies the NEXT file on the tape  as having a long linkname.*/
#define GOOD_USTAR_LONGLINK_TYPE 'K'
/** Identifies the NEXT file on the tape  as having a long name.*/
#define GOOD_USTAR_LONGNAME_TYPE 'L'

/**
 * TAR读。
*/
ssize_t good_tar_read(int fd, void *data, size_t size, good_buffer_t *buf);

/**
 * TAR写。
*/
ssize_t good_tar_write(int fd, const void *data, size_t size, good_buffer_t *buf);

/**
 * TAR写末尾。
*/
ssize_t good_tar_write_trailer(int fd, const void *data, size_t size, good_buffer_t *buf);

#endif //GOOD_UTIL_TAR_H
/*=========================================================================

  Program: GDCM (Grass Root DICOM). A DICOM library
  Module:  $URL$

  Copyright (c) 2006 Mathieu Malaterre
  Copyright (c) 1993-2005 CREATIS
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "gdcmSystem.h"

#include "md5.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream>

namespace gdcm
{

void process_file(const char *filename, md5_byte_t *digest)
{
  int di;
  size_t file_size, read;
  void *buffer;
  md5_state_t state;
  FILE *file = fopen(filename, "rb");

  /* go to the end */
  /*int*/ fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  /*int*/ fseek(file, 0, SEEK_SET);
  buffer = malloc(file_size);
  read = fread(buffer, 1, file_size, file);
  assert( read == file_size );

  md5_init(&state);
  md5_append(&state, (const md5_byte_t *)buffer, file_size);
  md5_finish(&state, digest);
  /*printf("MD5 (\"%s\") = ", test[i]); */
  for (di = 0; di < 16; ++di)
  {
    printf("%02x", digest[di]);
  }
  printf("\t%s\n", filename);
  free(buffer);
  fclose(file);
}

bool System::ComputeMD5(const char *buffer, const unsigned long buf_len,
  char *digest_str)
{
  if( !buffer || !buf_len )
    {
    return false;
    }
  md5_byte_t digest[16];
  md5_state_t state;
  md5_init(&state);
  md5_append(&state, (const md5_byte_t *)buffer, buf_len);
  md5_finish(&state, digest);

  //char digest_str[2*16+1];
  for (int di = 0; di < 16; ++di)
  {
    sprintf(digest_str+2*di, "%02x", digest[di]);
  }
  digest_str[2*16] = '\0';
  return true;
}

bool System::ComputeFileMD5(const char *filename, char *digest_str)
{
  // If not file exist
  // return false;
  md5_byte_t digest[16];

  /* Do the file */
  process_file(filename, digest);

  //char digest_str[2*16+1];
  for (int di = 0; di < 16; ++di)
  {
    sprintf(digest_str+2*di, "%02x", digest[di]);
  }
  digest_str[2*16] = '\0';
  return true;
}

int Mkdir(const char *pathname)
{
  (void)pathname;
  abort();
  return -1;
}

}

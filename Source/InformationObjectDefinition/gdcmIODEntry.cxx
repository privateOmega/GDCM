/*=========================================================================

  Program: GDCM (Grass Root DICOM). A DICOM library
  Module:  $URL$

  Copyright (c) 2006-2008 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "gdcmIODEntry.h"

#include <stdlib.h> // abort

namespace gdcm
{

IODEntry::UsageType IODEntry::GetUsageType() const
{
  assert( !Usage.empty() );
  if( Usage == "M" )
    {
    return Mandatory;
    }
  else if( Usage == "U" )
    {
    return UserOption;
    }
  else if( Usage.find( "C - " ) <  Usage.size() )
    {
    return Conditional;
    }
  //else
  abort();
  return Invalid;
}

}

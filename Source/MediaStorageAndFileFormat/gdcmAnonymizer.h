/*=========================================================================

  Program: GDCM (Grassroots DICOM). A DICOM library
  Module:  $URL$

  Copyright (c) 2006-2009 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __gdcmAnonymizer_h
#define __gdcmAnonymizer_h

#include "gdcmFile.h"
#include "gdcmSubject.h"
#include "gdcmEvent.h"

namespace gdcm
{
class  AnonymizeEvent : public AnyEvent {
public:
  typedef AnonymizeEvent Self;
  typedef AnyEvent Superclass;
  AnonymizeEvent(Tag const &tag = 0):m_Tag(tag) {}
  virtual ~AnonymizeEvent() {}
  virtual const char * GetEventName() const { return "AnonymizeEvent"; }
  virtual bool CheckEvent(const ::gdcm::Event* e) const
    { return dynamic_cast<const Self*>(e); }
  virtual ::gdcm::Event* MakeObject() const
    { return new Self; }
  AnonymizeEvent(const Self&s) : AnyEvent(s){};
private:
  void operator=(const Self&);
  Tag m_Tag;
};


/**
 * \brief Anonymizer
 * This class is a multi purpose anonymizer. It can work in 2 mode:
 * - dumb mode
 * - smart mode (implement the Basic Application Level Confidentiality Profile, DICOM PS 3.15-2008)
 * 
 * 1. dumb mode
 * This is a dumb anonymizer implementation. All it allows user is simple operation such as:
 * Tag based functions:
 * - complete removal of DICOM attribute (Remove)
 * - make a take empty, ie make it's length 0 (Empty)
 * - replace with another string-based value (Replace)
 * 
 * DataSet function:
 * - Remove all group length attribute from a DICOM dataset (Group Length element are deprecated, DICOM 2008)
 * - Remove all private attributes
 *
 * All function calls actually execute the user specified request. Previous implementation were calling 
 * a general Anonymize function but traversing a std::set is O(n) operation, while a simple user specified
 * request is O(log(n)) operation. So 'm' user interaction is O(m*log(n)) which is < O(n) complexity.
 *
 * 2. smart mode
 * this mode implements the Basic Application Level Confidentiality Profile (DICOM PS 3.15-2008)
 * In this case it is extremely important to use the same gdcm::Anonymizer class when anonymizing 
 * a FileSet. Once the gdcm::Anonymizer is destroyed its memory of known (already processed) UIDs
 * will be lost. which will make the anonymizer behaves incorrectly for attributes such as Series UID
 * Study UID where user want some consistancy.
 * When attribute is Type 1 / Type 1C, a dummy generator will take in the existing value and produce
 * a dummy value (a sha1 representation). sha1 algorithm is considered to be cryptograpgically strong
 * (compared to md5sum) so that we meet the following two conditions:
 *  - Produce the same dummy value for the same input value
 *  - do not provide an easy way to retrieve the original value from the sha1 generated value
 *
 * \see CryptographicMessageSyntax
 */
class CryptographicMessageSyntax;
class GDCM_EXPORT Anonymizer : public Subject
{
public:
  Anonymizer():F(new File),/*AESKey(),*/CMS(NULL) {}
  ~Anonymizer();

  /// Make Tag t empty (if not found tag will be created)
  /// Warning: does not handle SQ element
  bool Empty( Tag const &t );

  /// remove a tag (even a SQ can be removed)
  bool Remove( Tag const &t );

  /// Replace tag with another value, if tag is not found it will be created:
  /// WARNING: this function can only execute if tag is a VRASCII
  bool Replace( Tag const &t, const char *value );

  /// when the value contains \0, it is a good idea to specify the length. This function
  /// is required when dealing with VRBINARY tag
  bool Replace( Tag const &t, const char *value, VL const & vl );

  /// Main function that loop over all elements and remove private tags
  bool RemovePrivateTags();

  /// Main function that loop over all elements and remove group length
  bool RemoveGroupLength();

  /// Main function that loop over all elements and remove retired element
  bool RemoveRetired();

  // TODO:
  // bool Remove( PRIVATE_TAGS | GROUP_LENGTH | RETIRED );

  /// Set/Get File
  void SetFile(const File& f) { F = f; }
  //const File &GetFile() const { return *F; }
  File &GetFile() { return *F; }

  /// PS 3.15 / E.1.1 De-Identifier
  /// An Application may claim conformance to the Basic Application Level Confidentiality Profile as a deidentifier
  /// if it protects all Attributes that might be used by unauthorized entities to identify the patient.
  /// NOT THREAD SAFE
  bool BasicApplicationLevelConfidentialityProfile(bool deidentify = true);

  /// Set/Get AES key that will be used to encrypt the dataset within BasicApplicationLevelConfidentialityProfile
  /// Warning: set is done by copy (not reference)
  //void SetAESKey(AES const &aes);
  //const AES &GetAESKey() const;

  void SetCryptographicMessageSyntax( CryptographicMessageSyntax *cms );
  const CryptographicMessageSyntax *GetCryptographicMessageSyntax() const;

protected:
  // Internal function used to either empty a tag or set it's value to a dummy value (Type 1 vs Type 2)
  bool BALCPProtect(DataSet &ds, Tag const & tag);
  bool CanEmptyTag(Tag const &tag);
  void RecurseDataSet( DataSet & ds );

private:
  bool BasicApplicationLevelConfidentialityProfile1();
  bool BasicApplicationLevelConfidentialityProfile2();

private:
  // I would prefer to have a smart pointer to DataSet but DataSet does not derive from Object...
  SmartPointer<File> F;
  //AES AESKey;
  CryptographicMessageSyntax *CMS;
};

/**
 * \example ManipulateFile.cs
 * This is a C# example on how to use gdcm::Anonymizer
 */

} // end namespace gdcm

#endif //__gdcmAnonymizer_h

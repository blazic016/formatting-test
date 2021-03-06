/////////////////////////////////////////////////////////////////////////////
// Name:        xml.h
// Purpose:     wxXmlDocument - XML parser & data holder class
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id: xml.h,v 1.5.2.1 2002/12/21 13:35:48 VS Exp $
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XML_H_
#define _WX_XML_H_


#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/string.h>
#include <wx/object.h>
#include <wx/list.h>

class dmsDateTimeSpan;

class wxXmlDocument;


#include "Conversion.h"
#include "Validator.h"

#ifdef WXXMLISDLL
#define WXXMLDLLEXPORT WXDLLEXPORT
#else
#define WXXMLDLLEXPORT
#endif

class WXXMLDLLEXPORT wxXmlNode;
class WXXMLDLLEXPORT wxXmlProperty;
class WXXMLDLLEXPORT wxXmlDocument;
class WXXMLDLLEXPORT wxXmlIOHandler;
class wxInputStream;
class wxOutputStream;
class WXXMLDLLEXPORT wxDateTime;


// Represents XML node type.
enum wxXmlNodeType
{
    // note: values are synchronized with xmlElementType from libxml
    wxXML_ELEMENT_NODE       =  1,
    wxXML_ATTRIBUTE_NODE     =  2,
    wxXML_TEXT_NODE          =  3,
    wxXML_CDATA_SECTION_NODE =  4,
    wxXML_ENTITY_REF_NODE    =  5,
    wxXML_ENTITY_NODE        =  6,
    wxXML_PI_NODE            =  7,
    wxXML_COMMENT_NODE       =  8,
    wxXML_DOCUMENT_NODE      =  9,
    wxXML_DOCUMENT_TYPE_NODE = 10,
    wxXML_DOCUMENT_FRAG_NODE = 11,
    wxXML_NOTATION_NODE      = 12,
    wxXML_HTML_DOCUMENT_NODE = 13
};


// Represents node property(ies).
// Example: in <img src="hello.gif" id="3"/> "src" is property with value
//          "hello.gif" and "id" is prop. with value "3".

class WXXMLDLLEXPORT wxXmlProperty
{
public:
    wxXmlProperty() : m_next(NULL) {}
    wxXmlProperty(const wxString& name, const wxString& value,
                  wxXmlProperty *next)
            : m_name(name), m_value(value), m_next(next) {}

    wxString GetName() const { return m_name; }
    wxString GetValue() const { return m_value; }
    wxXmlProperty *GetNext() const { return m_next; }

    void SetName(const wxString& name) { m_name = name; }
    void SetValue(const wxString& value) { m_value = value; }
    void SetNext(wxXmlProperty *next) { m_next = next; }

private:
    wxString m_name;
    wxString m_value;
    wxXmlProperty *m_next;
};



// Represents node in XML document. Node has name and may have content
// and properties. Most common node types are wxXML_TEXT_NODE (name and props
// are irrelevant) and wxXML_ELEMENT_NODE (e.g. in <title>hi</title> there is
// element with name="title", irrelevant content and one child (wxXML_TEXT_NODE
// with content="hi").
//
// If wxUSE_UNICODE is 0, all strings are encoded in the encoding given to Load
// (default is UTF-8).

class WXXMLDLLEXPORT wxXmlNode
{
public:
    bool ReadTmpString(const wxString &name, wxString *value, bool logMissing);
public:
    wxXmlNode() : m_properties(NULL), m_parent(NULL),
                  m_children(NULL), m_next(NULL), m_doc(NULL) {}
    wxXmlNode(wxXmlNode *parent,wxXmlNodeType type,
              const wxString& name, const wxString& content,
              wxXmlProperty *props, wxXmlNode *next);
    virtual ~wxXmlNode();

    // copy ctor & operator=. Note that this does NOT copy syblings
    // and parent pointer, i.e. m_parent and m_next will be NULL
    // after using copy ctor and are never unmodified by operator=.
    // On the other hand, it DOES copy children and properties.
    wxXmlNode(const wxXmlNode& node);
    wxXmlNode& operator=(const wxXmlNode& node);

    // user-friendly creation:
    wxXmlNode(wxXmlNodeType type, const wxString& name,
              const wxString& content = wxEmptyString);
    void AddChild(wxXmlNode *child);
    void InsertChild(wxXmlNode *child, wxXmlNode *before_node);
    bool RemoveChild(wxXmlNode *child);
    void AddProperty(const wxString& name, const wxString& value);
    bool DeleteProperty(const wxString& name);

    // access methods:
    wxXmlNodeType GetType() const { return m_type; }
    wxString GetName() const { return m_name; }
    wxString GetLongName(const wxString &name="") const;
    wxString GetContent() const { return m_content; }
    wxString GetValue();
    int      GetValue(int def);

    wxXmlNode *GetParent() const { return m_parent; }
    wxXmlNode *GetNext() const { return m_next; }
    wxXmlNode *GetChildren() const { return m_children; }

    wxXmlProperty *GetProperties() const { return m_properties; }
    bool GetPropVal(const wxString& propName, wxString *value) const;
    wxString GetPropVal(const wxString& propName,
                        const wxString& defaultVal) const;
    bool HasProp(const wxString& propName) const;

    void SetType(wxXmlNodeType type) { m_type = type; }
    void SetName(const wxString& name) { m_name = name; }
    void SetContent(const wxString& con) { m_content = con; }

    void SetParent(wxXmlNode *parent) { m_parent = parent; }
    void SetNext(wxXmlNode *next) { m_next = next; }
    void SetChildren(wxXmlNode *child) { m_children = child; }

    void SetProperties(wxXmlProperty *prop) { m_properties = prop; }
    void AddProperty(wxXmlProperty *prop);

    // Debut ajouts DMS
    void Trace(const wxString &level="");

    wxXmlNode *AddNewChild(const wxString &name);
    wxXmlNode *AddNewChild(const wxString &name, const wxString &value);

    bool DeleteChild(wxXmlNode *node);

    wxXmlNode *AssumePath(const wxString &path);

    bool NameEQI(const wxString &name);

    wxXmlNode     *Find(const wxString &name, bool logError=false);
    bool           Find(const wxString &name, wxXmlNode *&node);
    wxXmlProperty *FindProperty(const wxString &name);

    bool Remove(wxXmlNode *node, bool *keep);
    bool Remove(const wxString &name, bool *keep);
    bool Remove(const wxString &name);

    bool Read(const wxString &name, wxString *value);
    bool ReadSFN(const wxString &name, wxString *value);
    bool Read(const wxString &name, u64 *value);
    bool Read(const wxString &name, int *value);
    bool Read(const wxString &name, int *value, int DefaultValue);
    bool Read(const wxString &name, int *value, const wxString &labelName, int labelValue);
    bool Read(const wxString &name, unsigned int *value);
    bool Read(const wxString &name, unsigned long *value);
    bool Read(const wxString &name, int *value, const char *valueList[]);
    bool Read(const wxString &name, u64 *value, dmsNameRank valueList[]);
    bool Read(const wxString &name, unsigned short *value);
    bool Read(const wxString &name, unsigned char *value);
    bool Read(const wxString &name, bool *value);
    bool Read(const wxString &name, dmsBuffer *value);
    bool ReadAscii(const wxString &name, dmsBuffer *value, bool withZero=true);
    bool Read(const wxString &name, wxDateTime *value);
    bool Read(const wxString &name, dmsDateTimeSpan *value);

    void Write(const wxString &name, const wxString &value, bool *keep = NULL);
    void Write(const wxString &name, int value, bool *keep = NULL);

    void SetDoc(wxXmlDocument *doc);
    void LoadVars();
    void ReplaceVars(wxString &value);
    bool DisplayUnused();
    void Used(){m_used=true;}

    // Fin ajouts DMS

private:
    wxXmlNodeType m_type;
    wxString      m_name;
    wxString      m_content;
    wxXmlProperty *m_properties;
    wxXmlNode     *m_parent, *m_children, *m_next, *m_lastchildren;
    bool          m_used;
public:
    wxXmlDocument *m_doc;

    void DoCopy(const wxXmlNode& node);
};







// This class holds XML data/document as parsed by XML parser.

class WXXMLDLLEXPORT wxXmlDocument : public wxObject
{
public:
    bool m_bLogMissing;
    bool m_bLoaded;
    bool m_bLoadComments;

public:
    wxXmlDocument();
    wxXmlDocument(const wxString& filename,
                  const wxString& encoding = wxT("UTF-8"));
    wxXmlDocument(wxInputStream& stream,
                  const wxString& encoding = wxT("UTF-8"));
    virtual ~wxXmlDocument();

    wxXmlDocument(const wxXmlDocument& doc);
    wxXmlDocument& operator=(const wxXmlDocument& doc);

    // Parses .xml file and loads data. Returns TRUE on success, FALSE
    // otherwise.
    bool Load(const wxString& filename, const wxString& encoding = wxT("UTF-8"));
    wxXmlNode* LoadText(const wxString& text,
                        const wxString& rootname = "",
                        const wxString& encoding = wxT("UTF-8"));
    bool Load(wxInputStream& stream, const wxString& encoding = wxT("UTF-8"));

    wxXmlNode* LoadRoot(const wxString& filename,
                        const wxString& rootname,
                        const wxString& encoding = wxT("UTF-8"));

    // Saves document as .xml file.
    bool Save(const wxString& filename) const;
    bool Save(wxOutputStream& stream) const;
    void SetLogMissing(bool value);

    bool IsOk() const { return m_root != NULL; }

    // Returns root node of the document.
    wxXmlNode *GetRoot() const { return m_root; }

    // Returns version of document (may be empty).
    wxString GetVersion() const { return m_version; }
    // Returns encoding of document (may be empty).
    // Note: this is the encoding original file was saved in, *not* the
    // encoding of in-memory representation!
    wxString GetFileEncoding() const { return m_fileEncoding; }

    // Write-access methods:
    void SetRoot(wxXmlNode *node) { delete m_root ; m_root = node; m_root->m_doc = this;}
    void SetVersion(const wxString& version) { m_version = version; }
    void SetFileEncoding(const wxString& encoding) { m_fileEncoding = encoding; }

#if !wxUSE_UNICODE
    // Returns encoding of in-memory representation of the document
    // (same as passed to Load or ctor, defaults to UTF-8).
    // NB: this is meaningless in Unicode build where data are stored as wchar_t*
    wxString GetEncoding() const { return m_encoding; }
    void SetEncoding(const wxString& enc) { m_encoding = enc; }
#endif

private:
    wxString   m_version;
    wxString   m_fileEncoding;
#if !wxUSE_UNICODE
    wxString   m_encoding;
#endif
    wxXmlNode *m_root;
public:
    wxArrayString *m_varNames;
    wxArrayString *m_varValues;

    void DoCopy(const wxXmlDocument& doc);
};

/* ------------------------------------------------------------------------

   ------------------------------------------------------------------------ */

class dmsXmlContext : public dmsvContext
{
public:
    wxString m_oPath;

public:
    dmsXmlContext(dmsvContextManager* manager, dmsValidator* mapping, const wxString &path);
    virtual ~dmsXmlContext();

    bool GetValue(wxString &value);
    bool SetValue(const wxString &value);


    void ShowError();
};

/* ------------------------------------------------------------------------

   ------------------------------------------------------------------------ */

class dmsXmlContextManager : public dmsvContextManager
{
public:
    wxXmlNode* m_poNode;

public:
    dmsXmlContextManager(wxXmlNode *node);
    virtual ~dmsXmlContextManager();

    dmsXmlContext *Add(const wxString &path, dmsValidator* mapping);
};


#endif // _WX_XML_H_

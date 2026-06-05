# RQMT Grammar

This document provides a parser-oriented description of the RQMT language.

Normative language requirements are defined in `specification.rqmt`.
Where discrepancies exist, the specification shall take precedence.

## Introduction

RQMT is an indentation-based language for the specification of structured requirements.

The grammar presented herein describes the syntactic structure recognized by compliant parsers. Certain constraints, such as identifier uniqueness, reference resolution, and indentation semantics, cannot be expressed purely through grammar productions and shall therefore be validated during semantic analysis.

---

## Lexical Elements

```text
requirement_keyword
    := "req"
     | "fr"
     | "nfr"
     | "br"
     | "ur"
     | "sr"

attribute_keyword
    := "id"
     | "author"
     | "date"
     | "title"
     | "description"
     | "type"
     | "status"
     | "priority"
     | "tags"
     | "alias"

identifier
    := positive_integer

alias
    := identifier_text

component
    := identifier_text
```

### References

```text
reference
    := "{"
       reference_path
       "}"
```

```text
reference_path
    := alias
     | relative_reference_path
```

```text
relative_reference_path
    := relative_prefix
       path_component_list
       "."
       alias
```

```text
relative_prefix
    := "."
     | ".."
     | "..."
     | ...
```

```text
path_component_list
    := component
     | component "." path_component_list
```

The final component of a reference path shall identify a requirement alias.

All preceding components shall identify files or directories.

Examples:

```rqmt
{authentication}

{.requirements.authentication}

{..safety.estop}

{...project.common.logging}
```

### Decorators WIP
```rqmt
@draft

@blocked
```

### Tags WIP
comma separated tags composed of charset [A-Za-z0-9_\-&], within the definition the may be comma- space- or new-line-separated. Multiple definitions of tags may be added, these will result 
```rqmt
req My Requirement:
    [tag1, tag2, ...]
   
fr My Functional Requirement:
    [tag1 tag2 ...]

nfr My Non-Functional Requirement:
    [
        tag1
        tag2
        tag3
    ]

req My Requirement:
    [tag1]
    [tag2]
    [...]
```

---

## Document Structure

```text
document
    := document_node*
```

```text
document_node
    := requirement
     | attribute
     | document_text
```

Root-level text shall produce document text nodes attached directly to the document node.

---

## Document Text

```text
document_text
    := markdown_text
```

Text appearing at the document root and not interpreted as a requirement declaration or attribute declaration shall be parsed as document text.

Document text shall be interpreted as Markdown.

Example:

```rqmt
author: Jonas T. Ulbrich

# Introduction

This document defines the RQMT language.
```

---

## Requirement Declaration

```text
requirement
    := requirement_keyword
       [ requirement_alias ]
       [ requirement_id ]
       [
           requirement_title
           [ requirement_brief_description ]
         | requirement_brief_description
       ]
       newline
```

Examples:

```rqmt
req

req Login:

req The system shall authenticate users.

req Login: The system shall authenticate users.

req{authentication}-1 Login:
```

---

## Requirement Alias

```text
requirement_alias
    := "{"
       alias
       "}"
```

Examples:

```rqmt
req{authentication}

fr{safety}
```

---

## Requirement Identifier

```text
requirement_id
    := "-"
       identifier
```

Examples:

```rqmt
req-1

fr-42
```

---

## Combined Alias And Identifier

```text
combined_identifier
    := requirement_alias
       requirement_id
```

Examples:

```rqmt
req{authentication}-1
```

---

## Title

```text
requirement_title
    := text_before_first_unescaped_colon
```

Examples:

```rqmt
req Login:

fr Authentication:
```

Titles shall be restricted to a single logical line.

The first unescaped colon (`:`) following a requirement declaration shall terminate the title.

A title may be followed by a brief description on the same logical line.

Examples:

```rqmt
req Login:
```

```text
title = "Login"
brief_description = null
```

```rqmt
req Login: Authenticate users
```

```text
title = "Login"
brief_description = "Authenticate users"
```

---

## Brief Description

```text
requirement_brief_description
    := description_text
```

Examples:

```rqmt
req The system shall authenticate users before granting access.

fr{login}-2 Login: The system shall provide a login interface.
```

Brief descriptions shall be restricted to a single logical line.

Brief descriptions shall be interpreted as plain text.

If no unescaped colon is present, all text following the requirement declaration shall be interpreted as a brief description.

Literal colons may be escaped using:

```text
\:
```

Example:

```rqmt
req HTTP\: Authentication Support
```

```text
title = null

brief_description =
    "HTTP: Authentication Support"
```

Once a title has been identified, all remaining text on the same logical line shall be interpreted as the brief description.

Additional colons within the brief description shall be treated as literal characters.

Example:

```rqmt
req HTTP: Authentication: only for restricted content
```

```text
title = "HTTP"

brief_description =
    "Authentication: only for restricted content"
```

---

## Attributes

```text
attribute
    := attribute_keyword
       ":"
       attribute_value
```

Examples:

```rqmt
author: Jonas T. Ulbrich

date: 2026-06-05
```

```rqmt
description: The system shall provide
    a login interface.
```

Attribute values may span multiple lines.

---

## Description

```text
description
    := markdown_text
```

Description content shall be interpreted as Markdown.

Descriptions are associated with requirements through indentation and parent-resolution rules.

Example:

```rqmt
req Login:

    The system shall authenticate users.

    Additional Markdown content may be provided.
```

---

## Hierarchy

```text
node
    := requirement
     | attribute
     | description
     | document_text
```

Parent-child relationships are determined by indentation.

A node shall become a child of the nearest valid node with a lower indentation level.

Hierarchy resolution is a semantic operation and is therefore not fully expressed by grammar productions.

Example:

```rqmt
req Authentication:

    author: Jonas T. Ulbrich

    The system shall authenticate users.

    req Login:
```

---

## Semantic Constraints

The following constraints are not enforced by grammar rules and shall be validated during semantic analysis:

* Requirement identifiers shall be unique within a document.
* Aliases shall be unique within a document.
* References shall resolve successfully.
* References shall resolve to requirements only.
* Requirement identifiers shall be positive integers.
* Parent-child relationships shall satisfy indentation rules.
* Invalid references shall generate diagnostics.
* Duplicate identifiers shall generate diagnostics.
* Duplicate aliases shall generate diagnostics.

---

## Parsing Notes

Requirement declarations are intentionally designed to be deterministic.

The parser shall distinguish titles from brief descriptions according to the following rules:

* The first unescaped colon following a requirement declaration terminates the title.
* If no unescaped colon exists, all remaining text shall be interpreted as a brief description.
* Escaped colons (`\:`) shall be treated as literal characters.
* Once a title has been identified, all remaining text on the logical line shall be interpreted as the brief description.

Examples:

```rqmt
req Login:
```

```text
title = "Login"
brief_description = null
```

```rqmt
req Login
```

```text
brief_description = "Login"
```

```rqmt
req Login: Authenticate users
```

```text
title = "Login"
brief_description = "Authenticate users"
```

```rqmt
req HTTP: Authentication: restricted content only
```

```text
title = "HTTP"

brief_description =
    "Authentication: restricted content only"
```

```rqmt
req HTTP\: Authentication
```

```text
brief_description =
    "HTTP: Authentication"
```

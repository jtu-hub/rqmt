# The RQMT Language

RQMT is a structured language for writing requirements.

Its design is guided by four principles:

1. Requirements should be human-readable.
2. Requirements should be machine-processable.
3. Requirements should be version-control friendly.
4. Requirements should support automated tooling.

RQMT documents are parsed into an Abstract Syntax Tree (AST), enabling validation, traceability, diagnostics, and editor integration.

---

# A Minimal Example

```rqmt
req-1 Authentication:
    The system shall authenticate users before granting access.
```

This defines a requirement with:

* ID: `1`
* Type: Generic Requirement (`req`)
* Title: `Authentication`
* Description: `The system shall authenticate users before granting access.`

---

# Requirement Types

RQMT provides a set of predefined requirement types.

```rqmt
req Generic Requirement
fr Functional Requirement
nfr Non-Functional Requirement
br Business Requirement
ur User Requirement
sr Software Requirement
```

Example:

```rqmt
fr-10 Login:
    The system shall provide a user login interface.
```

---

# Requirement Structure

A requirement is composed of:

* Requirement type
* Unique identifier
* Optional alias
* Optional title
* Optional attributes
* Description
* Optional child requirements

Example:

```rqmt
fr-10 Login:
    author: Jonas T. Ulbrich

    The system shall provide a user login interface.
```

---

# Identifiers

Each requirement shall have a unique identifier within a document.

```rqmt
req-1 Authentication:
req-2 Login:
req-3 Authorization:
```

Identifiers are used for:

* References
* Traceability
* Navigation
* Diagnostics

Identifiers are the primary means of requirement identification.

Titles may change over time, identifiers should remain stable.

---

# Aliases

Aliases provide human-readable references.

```rqmt
req{authentication}-1 Authentication:
```

A requirement may then be referenced using:

```rqmt
See {authentication}
```

Aliases improve readability while preserving stable identifiers.

---

# Titles

Titles provide a concise summary of a requirement.

```rqmt
req-1 Authentication:
```

In this example:

```text
Title = Authentication
```

Titles are intended for human readers and have no semantic role in requirement identity.

---

# Descriptions

Descriptions define the content of a requirement.

```rqmt
req-1 Authentication:
    The system shall authenticate users before granting access.
```

Descriptions may span multiple lines.

```rqmt
req-1 Authentication:
    The system shall authenticate users before granting access.

    Authentication shall support both local and remote users.
```

Description text is interpreted as Markdown.

---

# Attributes

Requirements may contain attributes.

```rqmt
req-1 Authentication:
    author: Jonas T. Ulbrich
    date: 2026-06-01

    The system shall authenticate users.
```

Attributes provide structured metadata associated with a requirement.

---

# Hierarchy

RQMT uses indentation to define hierarchy.

```rqmt
req-1 Authentication:
    fr-2 Login:
        The system shall provide a login interface.

    fr-3 Logout:
        The system shall provide a logout interface.
```

Equivalent AST:

```text
Authentication
├─ Login
└─ Logout
```

---

# References

Requirements may reference other requirements.

```rqmt
req{authentication}-1 Authentication:
    ...

req-2 Login:
    Depends on {authentication}
```

References are resolved during analysis and can be used for navigation and traceability.

---

# Markdown Support

RQMT embeds Markdown syntax.

```rqmt
req-1 Authentication:
    The system shall support:

    - Username/password authentication
    - OAuth authentication
    - LDAP authentication
```

This allows rich descriptions while preserving structure.

---

# Document Structure

A typical RQMT document follows the pattern:

```rqmt
author: Jonas T. Ulbrich
date: 01.06.2026

req-1 Requirement:
    attribute: value

    Description

    req-2 Child Requirement:
        Description
```

---

# Next Steps

For a complete description of the syntax, see:

* `grammar.md`
* `architecture.md`
* `roadmap.md`

## C Compiler to MiMa

The MiMa is a theoretical CPU designed to teach students at KIT the basics of Assembly and CPU design.
This is a compiler that compiles to the extended MiMa-Assembly (including LDIV/STIV instructions)

This was made as a hobby project for fun.

### Generated Assembly

The generated assembly contains references to predefined variables/constants (__aux, __one, __m_one, __sp).
__aux is used in binary operations to store one of the two values.
__one and __m_one are constants in memory for 1 and -1 respectively, as the MiMa does not have INC and DEC instructions or the like.
__sp is the current stack pointer. This is used in calculations to store previous values, as the MiMa only has one general purpose register.

### Grammar

The grammar is C-like with additions/simplifications specific to MiMa Assembly (like to org directive)

#### Statements

```
s ::= var x [= n]; s
    | x = e; s
    | [org n] s
    | ε
```

#### Value Expressions

```
e ::= e2 [+ e]
e2 ::= -e3 | e3
e3 ::= n | x | (e)
```

```
n ∈ ℤ
x ∈ Var
```

#### Boolean Expressions (not supported)

```
b ::= b2 [|| b]
b2 ::= b3 [&& b2]
b3 ::= !b3 | b4
b4 ::= e bop e | (b) | true | false
bop ::= < | > | == | != | <= | >=
```
## Types naming
Types must be named following pascal case. Example: `TypeName`

## Functions
Functions must be named following pascal case. Example: `FunctionName`

### Interfaces

## Variable naming
Variables and class members must be named following camel case. Example: `variableName`.

## Indentation and spacing
Code must be indented with 4 spaces rather than tabs.

A space must be placed between sections. Example: spacing between `if` and the following condition.

Curly brackets defining and new scope must be placed on a new line. The following scope must be indented to a new level.

Examples:

```cpp
// Correct formatting
if (Condition)
{
    DoSomething();
}
```

```cpp
if(Condition)         
{       
DoSomething();        // Bad - This new scope must be indented to a new level
}
```

```cpp
if(Condition)         // Bad - There should be some spacing between IF and the bracket
{       
    DoSomething();
}
```

```cpp
if (Condition){       // Bad - The bracket must be placed on a newline
    DoSomething();
}
```

## Types naming
Types must be named following pascal case. Example: `TypeName`

### Interfaces
Types that can not be used directly, and must be extended by another must be prefixed by `I`. Example: `IVehicle` and `Car : IVehicle`

## Functions
Functions must be named following pascal case. Example: `FunctionName`

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

## Documentation
Publicly accessible functions must be documented. This should describe what the function does, how it must be used and what the result will be.

The documentation consists of a comment block before the function definition. An `@brief` tag is required to give a short description of the function.

All parameter must be listed by an `@param [ParameterName]` tag. Parameters meant as an output are marked by `@param[out] [ParameterName]`.

Where applicable, the return value must be explained by an `@return` tag.

Example:
```cpp
/**
 * @brief Attempt to add two integers together
 * @param A The first integer used for addition
 * @param B The second integer used for addition
 * @param[out] Output The resulting integer if addition was possible
 * @return Wether or not the addition was successful
 */
bool Add(int A, int B, int& Output);
```

### Warnings
Warning messages must be marked explicitly. This is done like this:

```cpp
// @warning This is a dangerous function
void DangerousFunction();
```

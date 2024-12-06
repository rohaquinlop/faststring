# faststring

`faststring` is a high-performance Python library designed to enhance string manipulation operations. It provides a `MString` class that offers efficient string concatenation, modification, slicing, and other common string operations, with the added benefit of mutable strings.

## Features

- **Mutable Strings**: Unlike native Python strings, which are immutable, `MString` allows you to modify strings in place.
- **Efficient Concatenation**: Append strings without the overhead of creating new string objects.
- **Index Modification**: Modify characters at specific indices with ease.
- **Slicing**: Extract and modify slices of strings efficiently.
- **Contains Check**: Quickly check if a substring is present.
- **Find Substrings**: Locate substrings within the string.
- **Clear Strings**: Reset the string content efficiently.
- **Multiplication**: Multiply the string by an integer using the `*` operator.

## Installation

You can install FastString using `pip`:

```sh
pip install faststring
```

## Usage

Here's a quick example to get you started:

```python
from faststring import MString

# Create a new MString object
s = MString("Hello, ")

# Append a string
s += "world!"

# Or use the append method
s.append(" How are you?")

# Modify a character at a specific index
s[7] = "W"

# Modify a slice of the string
s[0:2] = "Hi"
s[0:5] = "Hi"
s[0:2] = "Hello"

# Extract a slice of the string
print(s[0:6])  # Output: Hello,

# Check if a substring is present
print("World" in s)  # Output: True

# Find the index of a substring
print(s.find("World"))  # Output: 7

# Clear the string
s.clear()

# Output the final string
print(s)  # Output: ""

# You can also use the MString object as a regular string
print(len(s))  # Output: 0

# You can also convert the MString object to a regular string
s = s.to_string()

ms = MString("Hello")

# Multiply the string by an integer
ms *= 3
print(ms)  # Output: HelloHelloHello

# Reverse the string
ms.reverse()
print(ms)  # Output: olleHolleHolleH

# Replace a substring with another string
ms.replace("olleH", "Hi")
print(ms)  # Output: HiHiHi
```

## Mutable Strings

One of the key features of `faststring` is the ability to modify strings in place. This can be particularly useful when dealing with large strings or when you need to perform multiple string manipulation operations in sequence.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact

If you have any questions or suggestions, feel free to reach out to me at [rohaquinlop301@gmail.com](mailto:rohaquinlop301@gmail.com) or [LinkedIn](https://www.linkedin.com/in/robin-hafid/).

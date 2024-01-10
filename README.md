# Learn LLVM 17

<a href="https://www.packtpub.com/product/learn-llvm-17-second-edition/9781837631346?utm_source=github&utm_medium=repository&utm_campaign=9781837631346"><img src="https://m.media-amazon.com/images/I/71JS1fL+SkL._SL1500_.jpg" alt="Learn LLVM 17" height="256px" align="right"></a>

This is the code repository for [Learn LLVM 17](https://www.packtpub.com/product/learn-llvm-17-second-edition/9781837631346?utm_source=github&utm_medium=repository&utm_campaign=9781837631346), published by Packt.

**A beginner's guide to learning LLVM compiler tools and core libraries with C++**

## What is this book about?
LLVM was built to bridge the gap between the theoretical knowledge found in compiler textbooks and the practical demands of compiler development. With a modular codebase and advanced tools, LLVM empowers developers to build compilers with ease. This book serves as a practical introduction to LLVM, guiding you progressively through complex scenarios and ensuring that you navigate the challenges of building and working with compilers like a pro.

This book covers the following exciting features:
* Configure, compile, and install the LLVM framework
* Understand how the LLVM source is organized
* Discover what you need to do to use LLVM in your own projects
* Explore how a compiler is structured, and implement a tiny compiler
* Generate LLVM IR for common source language constructs
* Set up an optimization pipeline and tailor it for your own needs
* Extend LLVM with transformation passes and clang tooling
* Add new machine instructions and a complete backend

If you feel this book is for you, get your [copy](https://www.amazon.com/dp/1837631344) today!

<a href="https://www.packtpub.com/?utm_source=github&utm_medium=banner&utm_campaign=GitHubBanner"><img src="https://raw.githubusercontent.com/PacktPublishing/GitHub/master/GitHub.png" 
alt="https://www.packtpub.com/" border="5" /></a>

## Instructions and Navigations
All of the code is organized into folders.

The code will look like the following:
```
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/ToolOutputFile.h"
```

**Following is what you need for this book:**
This book is for compiler developers, enthusiasts, and engineers new to LLVM. C++ software engineers looking to use compiler-based tools for code analysis and improvement, as well as casual users of LLVM libraries who want to gain more knowledge of LLVM essentials will also find this book useful. Intermediate-level experience with C++ programming is necessary to understand the concepts covered in this book.

With the following software and hardware list you can run all code files present in the book (Chapter 1-13).
### Software and Hardware List
| Chapter | Software required | OS required |
| -------- | ------------------------------------ | ----------------------------------- |
| 1-13 | A C/C++ compiler: gcc 7.1.0 or later, clang 5.0 or later, Apple clang 10.0 or later, Visual Studio 2019 16.7 or later | Linux (any), Windows, Mac OS X, or FreeBSD |
| 1-13 | CMake 3.20.0 or later |  |
| 1-13 | Ninja 1.11.1 |  |
| 1-13 | Python 3.6 or later |  |
| 1-13 | Git 2.39.1 or later |  |


### Related products
* LLVM Techniques, Tips, and Best Practices Clang and Middle-End Libraries [[Packt]](https://www.packtpub.com/product/llvm-techniques-tips-and-best-practices-clang-and-middle-end-libraries/9781838824952?utm_source=github&utm_medium=repository&utm_campaign=9781838824952) [[Amazon]](https://www.amazon.com/dp/1838824952)

* C++20 STL Cookbook [[Packt]](https://www.packtpub.com/product/c20-stl-cookbook/9781803248714?utm_source=github&utm_medium=repository&utm_campaign=9781803248714) [[Amazon]](https://www.amazon.com/dp/1803248718)


## Get to Know the Authors
**Kai Nacke**
is a professional IT architect currently residing in Toronto, Canada. He holds a diploma in computer science from the Technical University of Dortmund, Germany. and his diploma thesis on universal hash functions was recognized as the best of the semester. With over 20 years of experience in the IT industry, Kai has extensive expertise in the development and architecture of business and enterprise applications. In his current role, he evolves an LLVM/clang-based compiler. For several years, Kai served as the maintainer of LDC, the LLVM-based D compiler. He is the author of D Web Development and Learn LLVM 12, both published by Packt. In the past, he was a speaker in the LLVM developer room at the Free and Open Source Software Developers’ European Meeting (FOSDEM). 


**Amy Kwan**
is a compiler developer currently residing in Toronto, Canada. Originally, from the Canadian prairies, Amy holds a Bachelor of Science in Computer Science from the University of Saskatchewan. In her current role, she leverages LLVM technology as a backend compiler developer. Previously, Amy has been a speaker at the LLVM Developer Conference in 2022 alongside Kai Nacke.

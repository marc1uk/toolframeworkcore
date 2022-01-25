![ToolFramework](https://user-images.githubusercontent.com/14093889/147487041-eff81a5d-45e4-4f3c-9cde-8066b9726659.png)

ToolFramework is an open source general modular C++ Framework.


# *PLEASE NOTE: This is the core framework only!!! do not clone this repo for building your own application.
To create your own ToolFramework appliciaion please clone/fork the ToolApplication repository https://github.com/ToolFramework/ToolApplication which has a script ```GetToolFramework.sh``` to pull this repository down as a dependancy and set up everything for you.

****************************
# Concept
****************************

The main executable creates a ToolChain which is an object that holds Tools. Tools are added to the ToolChain and then the ToolChain can be told to Initialise Execute and Finalise each Tool in the chain.

The ToolChain also holds a uesr defined DataModel which each Tool has access too and can read ,update and modify. This is the method by which data is passed between Tools.

User Tools can be generated for use in the ToolChain by included scripts.

For more information consult the ToolFramework manual:

https://docs.google.com/document/d/18rgYMOAGt3XiW9i0qN9kfUK1ssbQgLV1gQgG3hyVUoA

or the Doxygen docs

docs https://toolframework.github.io/ToolFrameworkCore/


Copyright (c) 2016 Benjamin Richards (benjamin.richards@warwick.ac.uk)

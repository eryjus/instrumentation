# Bochs Instrumenatation for CenturyOS

Welcome to the instrumentation package for Bochs.  This project was developed to assist in debugging CenturyOS.

The goal here is to produce enough meaningful output on the execution details to allow for some tracing of complex situations that are otherwise difficult to identify.

As usual, this journal file will be used to follow my thoughts (just as with any project).  History will not be changed and the purpose is different than that of comments in the source.

---

## Version 0.0.1

This is the initial version.  The goals here are simple:
* Get Bochs and the instrumentation package to compile
* Set up my own host system to properly take avantage of this new tool
* Turn off instrumentation by default and then add controls to be able to toggle the instrumentation status at runtime

I will start with a copy of Bochs `example1`, but will end up replacing (nearly?) everything with my own code.

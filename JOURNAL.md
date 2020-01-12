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

### 2020-Jan-11

So I started by getting the necessary base files in order (README.md, JOURNAL.md, LICENSE.md, etc.) and and initial commit for github. 

Now, I am on to the toggle portion of the code.  This should be relatively simple, but it appears not to be trivial.  

I have been able to confirm that the code I am looking for is: `87 d2  xchg   %edx,%edx`.  But I need to do some more research....  

The element I believe I should be investigating is:

```C++
  unsigned opcode_length;
  Bit8u    opcode[MAX_OPCODE_LENGTH];
```

I expect that this contains the code actual machine code for the CPU to emulate.

---

I was able to get the code working properly.  To do this, I had to go back one more level of abstraction into the Bochs instruction structure.  I am now able to enable/disable by toggle the instrumentation dynamically.

I will commit this version.

---

## Version 0.0.2

In this version, I will change the output to be to an `instrument.log` file.  This is because there is far more information being output that will fit on a screen's rollback buffer and I do not want to have multiple windows open while I redirect output to a file and `tail -f` it from another window (and respond to the debugging prompts in the first!).

I was able to reformat the output so that I am using the Bochs logging API.  In this manner, the `.bochsrc` can control where this information is written.

So, with the option `log: -` the instrumentation output is sent to `stderr` whereas the option `log: file-name` the instrumentation output is sent to `file-name`.

This, then, leaves the flexibility to the user.  Committing this code.



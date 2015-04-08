# Travis J. Sanders Code Portfolio

I recently completed a full Computer Science degree in fifteen months. This is a brief summary of the skills I developed and several of the projects I completed in that time.

## C/C++

### I am an extremely quick study
After one month of C++ experience, I wrote this [Game of Life](https://github.com/tjsander/Life/blob/master/life.cpp) implementation as an exercise in arrays.

By the end of my term introductory course, I completed a final project in object-oriented design: a simple [dungeon maze RPG with a Battleship Easter Egg.](https://github.com/tjsander/CS165-Final-Project)

### System-Level and Networking
During an intensive Operating Systems course, I completed large projects using:
- File IO to [replicate the Unix `myar` command](assembly/myar)
- Threads and forked processes in parallel computing project to [compute 100k+ twin prime numbers](assembly/multiproc_twinprime)
- Sockets and other network communications (which I expanded upon in a later networking project)

Finally, I wrote an in-depth [comparison of multiprocessing methods in Win32 and POSIX.](http://tjsander.github.io/windows-api.html)

### Testing
Before I started in programming, I already had extensive work experience in software testing at WB Games Inc.

Recently, I wrote a test squite using both unit and randomized testing for an extremely buggy C implementation of the Dominion card game.

## Python

Years before starting my Computer Science degree, with no prior experience in Python, I sat down and wrote a Twitter bot to scrape exchange rate data from a bank web site and post the results daily. Unfortunately, this code no longer works with the new Twitter API and is somewhat embarassing to look at now.

Since this experience, Python has become my weapon of choice for all projects without language requirements, especially those involving system operations.

I used the Python Google App Engine API to create a [cloud address book.](http://brave-cursor-751.appspot.com/) 

## x86 Assembly

Unlike approximately 95% of my peers, I loved learning MASM. Something about knowing exactly what instructions the processor is receiving and executing is very satisfying to me.

*The following examples depen on the Irvine32 library.*

- [Prime number generator](assembly1.asm) and exercise in array usage.
- [Implementation of input/output functionality](assembly2.asm) and string to integer conversion using recursive assembly processes.

I am currently studying ARM assembly on the Raspberry Pi and hope to be working on the Pebble Time smartwatch platform soon.

## Web Development and Mobile
I create simple, elegant, and user-friendly interfaces through mobile-first design principles. I integrate bootstrap CSS and Javascript into my web development projects to create consistent, flexable interfaces across all devices.

One early project I completed was [a time-relational "done and todo" list](http://web.engr.oregonstate.edu/~sandetra/final/demo.html).

### Database Design
A complex relational [Pokemon database](http://web.engr.oregonstate.edu/~sandetra/pokedex/) including types, a strength/weakness chart, user accounts and matchup analysis. (See also: my [schema](pokedex-definition.sql) and [queries](pokedex-queries.sql).)

### User Interface Design
I completed a course in usability engineering, during which I designed and tested an [UI for pixel artitsts.](pixelator_design.pdf)

## Java
During my liberal arts studies at Willamette University I took intro and data structures coureses in Java. Most recently, I worked on a team of three to write a small JUnit test suite for the Apache Commons URL Validator.

## Project Management
I have completed several group projects using many different development methods, including Spiral, Agile, Scrum and Waterfall.

In Analysis of Algorithms, I worked on a team of three for all programming projects. We used the [Cloud9](https://c9.io/) platform to collaboratively edit code in real time, which resulted in many hours of pair-programming and some very solid code. Our signature achievement was a travelling salesman algorithm that utilized greedy and two-opt swap methods to calculate a near-optimal solution quickly, placing within the top 5 of 25 teams in testing.

In my [capstone](https://github.com/lathamfell/AAAH), I acted as both the liason between the client and the team as well as in the roles of scrum master and developer. Our client was the Oregon State advising faculty and the task was to create a system to filter incoming appointment emails, generate an iCal event for their calendars and add the events to a mySQL database with a Python CLI.
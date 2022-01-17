# BMB - Technical Specification

# Overview

Basic Multipurpose Backend is a backend layer written in C++,  designed to be fast and modular. Its most important purpose is to provide backend infrastructure for my portfolio website. In the future it could be extended with new modules to work with other projects.

# Planned services

BMB is a modular piece of software. That means that it uses different services for a variety of purposes.

### Host service

The host service is the main "body" of BMB. It works as an API for all other services to connect to. It contains functions responsible for collecting data from modules such as markdown compiler and sending them to the database module. Then it retrieves new data from database and sends it to the frontend communication module.

### The first services that are needed

- Frontend module - gets HTTP GET requests from React frontend and responds to them.
- Database module - responsible for writing to and retrieving data from MySQL database.
- Markdown compiler service - mostly used to compile new blog posts from markdown to HTML code. It will watch a specific folder for new markdown files. Pretty easy huh?
- GitHub integration module - watches for new GitHub issues and calculates progress on projects that I'm currently working on. It is also responsible for sorting my projects according to which one I made a commit to most recently. It works every hour or so and communicates with database module, which then saves those changes to the projects table in database.

### Frontend module

It will use internet sockets for this purpose using send() to the socket's file descriptor which is just an integer. Reserving a socket is as easy as writing the following statement:

```cpp
// Socket descriptor is just an integer
int socketDescriptor = socket(int domain, int type, int protocol);
```

More info on internet sockets in Unix can be found under:

[Beej's Guide to Network Programming](https://www.notion.so/Beej-s-Guide-to-Network-Programming-17a4e537c8024313a0bc9470ad315df4)

**man send** and **man recv** manpages

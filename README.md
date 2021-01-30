# BMB - Technical Specification

## Overview

Basic Multipurpose Backend is a backend layer written in C++,  designed to be fast and modular. It's most important purpose is to provide backend infrastructure for my portfolio website. In the future it could be extended with new modules to work with other projects. Use of UNIX sockets constrains the deployment platform to UNIX and UNIX-like operating systems.

## Planned services

BMB is a modular piece of software. That means that it uses different services for a variety of purposes.

### Host service

The host service is the main "body" of BMB. It works as an API for all other services to connect to. It contains functions responsible for collecting data from modules such as markdown compiler and sending them to the database module. Then it retrieves new data from database and sends it to the frontend communication module.

### The first services that are needed

- Frontend module - gets HTTP GET requests from React frontend and sends POST requests to it.
- Database module - responsible for writing to and retreving data from MySQL database.
- Markdown compiler service - mostly used to compile new blog posts from markdown to html code. It will watch a specific folder for new markdown files. Pretty easy huh?
- GitHub integration module - watches for new GitHub issues and calculates progress on projects that I'm currently working on. It is also responsible for sorting my projects according to which one I made a commit to most recently. It works every hour or so and communicates with database module, which then saves those changes to the projects table in database.
# Gear S3 Wifi Messaging
### Author: Peter Swanson
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Python 2.7](https://img.shields.io/badge/Python-2.7-brightgreen.svg)](https://www.python.org/downloads/release/python-2714/)
[![Django 1.9.13](https://img.shields.io/badge/Django-1.9.13-brightgreen.svg)](https://pypi.org/project/Django/1.9.13/)

## Background
The Samsung Gear S3 Classic has no built-in messaging for iPhones

<b>This application will allow messages to be sent from the watch to both phones and emails via SMTP.</b>

## Setup
### Create a config.ini file:
This file should be placed in the <b>server/dispatch</b> directory (the directory that contains 
<b>manage.py</b>).

This file must adhere to the following format:
```
username=USERNAME
password=PASSWORD
server=SERVER
```
Where the lefthand value is your SMTP server username, password, and server respectively. 

Note: If you are using gmail, the last argument is optional. In this case the prior two arguments are your gmail
username and password respectively. 

## Functionality
### Running the server locally:
Begin by migrating the database.
```
$ python manage.py makemigrations
$ python manage.py migrate
```
Then run the server.
```
python manage.py runserver
```
You can visit the server at http://127.0.0.1:8000/, however it is meant to be a RESTful enpoint 
rather than a real website.

### Sending a message to/from the server:
For now, you can send a message via POST request with a valid CSRF token.
The <b>post_test.py</b> file records user input and sends it to the server as a POST request.
```
$ python post_test.py
```
Note: Requires the requests library (https://pypi.org/project/requests/)

## Files
 - server/dispatch/ - Contains Django server files
    - manage.py - Facilitates interaction with server
    - post_test.py - Test sending a POST request to the server
    - base/urls.py - Views to run when a url is visited
    - base/views.py - Functions to run or render content per url
    - base/models.py - Data structures in Django format
    - base/templates/ - HTML templates to render with dynamic content
    - dispatch/settings.py - Django project settings
    
## Requirements
- Python 2.7 - https://www.python.org/downloads/release/python-2714/

- Django 1.9.13 - https://pypi.org/project/Django/1.9.13/

- Optional (for post_test.py): Requests - https://pypi.org/project/requests/
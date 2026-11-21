sudoku-swi
----------

Lightweight Static Web Interface for sudoku generator, visible on
sudoku.coldfix.de_.

.. _sudoku.coldfix.de: http://sudoku.coldfix.de

The thing itself isn't really useful, but I had already coded the sudoku
generator a while back, so it wasn't much effort to provide a web-interface.
This is coded in illegably ugly PHP… shame on me. I am now setting the code
free, so you can judge for yourself.


Installation
~~~~~~~~~~~~

Build the generator:

.. code-block:: bash

    git clone https://github.com/coldfix/sudoku-swi
    cd sudoku-swi
    apt-get install base-devel boost-dev
    make

The build places the executable in ``bin/sudoku``, outside the webroot.
Keep ``bin/`` alongside ``html/`` when deploying, and remove any legacy
``html/sudoku`` executable. The web-server user needs execute permission on
the binary, but should not have write permission on it or ``bin/``.

Now, serve only the ``html/`` folder with CGI enabled. For example, use lighttpd
to serve on http://localhost:3030:

.. code-block:: bash

    apt-get install lighttpd php php-cgi fcgi
    lighttpd -Df lighttpd.conf

Web requests accept positive integer block dimensions whose product is at
most 25 (for example, ``5x5``, ``6x4``, or ``7x3``).


Docker
------

Build a docker image from source that runs the site using lighttpd::

    git clone https://github.com/coldfix/sudoku-swi
    cd sudoku-swi
    docker build . -t coldfix/sudoku

Or just pull the latest version from Docker Hub::

    docker pull coldfix/sudoku

And run the site as follows::

    docker run --rm --init -p 3000:3000 --cap-drop=all coldfix/sudoku

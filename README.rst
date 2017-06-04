sudoku-swi
----------

Lightweight Static Web Interface for sudoku generator, visible on
sudoku.coldfix.de_.

.. _sudoku.coldfix.de: http://sudoku.coldfix.de

This is coded in illegably ugly PHP… shame on me. Please, don't blame me for
the mistakes I've done years ago.

The thing itself isn't really useful, but I had already coded the sudoku
generator a while back when I was in school, so it wasn't much effort to
provide a web-interface. I am now setting the code free, so you can decide on
your own, whether you want to look at such ugliness.


Installation
~~~~~~~~~~~~

Make the ``html/`` folder accessible somewhere in your webroot, enable PHP.

Run ``make`` to build the generator_ and put binary in ``html/sudoku``.

.. _generator: https://github.com/coldfix/sudoku-cli


Docker
------

You can run the site inside a docker container using lighttpd:

.. code-block:: bash

    git clone https://github.com/coldfix/sudoku-swi
    cd sudoku-swi
    docker build . -t sudoku
    docker run -d --restart=always -p 3000:3000 sudoku

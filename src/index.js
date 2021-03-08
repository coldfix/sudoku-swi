import { Elm } from './Sudoku.elm'

var app = Elm.Sudoku.init({
  node: document.getElementById('sudoku'),
  flags: window.location.hash.slice(1) || "3x3",
});

window.addEventListener("hashchange", function() {
  app.ports.recvHash.send(window.location.hash);
}, false)

app.ports.fetchBoard.subscribe(function(size) {
  fetch(`sudoku.php?size=${size}`)
    .then(response => response.json())
    .then(data => {
      if (data.success) {
        app.ports.recvBoard.send({
          "data": data.board,
          "size": data.size,
        });
        window.location.hash = data.size;
      }
    });
});

var visible = false;

function initApp()
{
  window.addEventListener("hashchange", updateState, false)
  updateState();
}

function updateState()
{
  let size = window.location.hash;
  if (size === '') {
    size = '3x3';
  }
  else if (size.startsWith('#')) {
    size = size.substr(1);
  }
  renderSizeChoices(size);
  queryTable(size);
}

function renderSizeChoices(selected)
{
  let options = ["2x2", "3x3", "4x4"];
  if (!options.includes(selected)) {
    options.push(selected);
  }
  let select_node = document.querySelector("select[name='size']");
  select_node.innerHTML = '';
  for (const option of options) {
    let option_node = document.createElement("option");
    option_node.selected = option === selected;
    option_node.innerHTML = option;
    select_node.add(option_node);
  }
}

function queryTable(size)
{
  if (size === undefined) {
    size = "3x3";
  }
  fetch(`sudoku.php?size=${size}`)
    .then(response => response.json())
    .then(content => {
      if (content.success) {
        renderTable(content.board, content.rows, content.cols);
      }
    });
}

function renderTable(board, rows, cols)
{
  visible = false;
  document.querySelector("#showhidebutton").innerHTML = "Show solution";

  let table = document.createElement('table');

  let n = rows * cols - 1;
  let i = 0;
  for (const row of board) {
    let tr = document.createElement("tr");
    let j = 0;

    for (const cell of row) {
      let td = document.createElement("td");

      if (i > 0 && i % rows == 0)     { td.classList.add('top'); }
      if (i < n && (i+1) % rows == 0) { td.classList.add('bottom'); }
      if (j > 0 && j % cols == 0)     { td.classList.add('left'); }
      if (j < n && (j+1) % cols == 0) { td.classList.add('right'); }

      if (cell.startsWith('~')) {
        let span = document.createElement('span');
        span.className = 'hidden';
        span.innerHTML = cell.substr(1);
        td.appendChild(span);
      }
      else {
        td.innerHTML = cell;
      }

      tr.appendChild(td);
      ++j;
    }

    table.appendChild(tr);
    ++i;
  }

  document.querySelector("table.board").innerHTML = table.innerHTML;
}


function toggleSolution()
{
  var fields = document.querySelectorAll(".hidden");
  var button = document.querySelector("#showhidebutton");
  if (visible) {
    for (const node of fields) {
      node.style.display = 'none';
    }
    button.innerHTML = "Show solution";
  } else {
    for (const node of fields) {
      node.style.display = 'inline';
    }
    button.innerHTML = "Hide solution";
  }
  visible = !visible;
}


function changeSize(value)
{
  if (window.location.hash == '#' + value) {
    updateState();
  }
  else {
    window.location.hash = value;
  }
  return false;
}

window.onload = function () {
  // Add event listener to reload page when browser window is focused
  window.addEventListener("focus", function () {
    location.reload();
  });

  var segment0 = [100, 120];
  var segment1 = [350, 50];

  var point = [200, 120];

  var canvas = document.getElementById("canvas");
  var context = canvas.getContext("2d");

  context.strokeStyle = "black";
  // Draw a line between segment0 and segment1
  context.beginPath();
  context.moveTo(segment0[0], segment0[1]);
  context.lineTo(segment1[0], segment1[1]);
  context.stroke();
  context.closePath();

  // Draw point
  context.beginPath();
  context.arc(point[0], point[1], 5, 0, 2 * Math.PI);
  context.fillStyle = "red";
  context.fill();
  context.closePath();

  // Draw points for segment0 and segment1
  context.beginPath();
  context.arc(segment0[0], segment0[1], 5, 0, 2 * Math.PI);
  context.fillStyle = "blue";
  context.fill();
  context.closePath();

  context.beginPath();
  context.arc(segment1[0], segment1[1], 5, 0, 2 * Math.PI);
  context.fillStyle = "blue";
  context.fill();
  context.closePath();
};

// Is there an event for when the browser window is focused?
// https://stackoverflow.com/questions/1060008/is-there-an-event-for-when-the-browser-window-is-focused


static const char stylePage[] = R"~(
body, button,input {font-family:sans-serif; font-size: 22px;}
body {
    padding: 5% 5%;
}
a, button {
    display:inline-block;
    width:45%;
    background-color:#337ab7;
    text-align:center;
    border:1px solid #2e6da4;
    border-radius:4px;
    text-decoration:none;
    color:#fff;
    vertical-align:middle;
    padding: 12px 0;
}
a+a, button+button {
    margin-left:5%;
}
a:hover, button:hover {
    background-color:#23527c;
}
input {
    display:inline-block;
    width: 30%;
    margin-bottom:12px;
}
.header-fixed {
    width:100%;
    position:fixed;
    top:0px;
    left:0px;
    background-color:#337ab7;
    color:#fff;
    padding: 5px;
}
.content {
    padding-top: 15px;
    text-align: center;
}
.button30 {
    width: 28%;
}
)~";


static const char mainPage[] = R"~(
<!DOCTYPE html>
<meta name="viewport" content="width=device-width, initial-scale=1">
<html>
<head>
<title>Logobot</title>
<link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
<div class="header-fixed">Logobot: <span id="state">ready!</span></div>

<div class="content">

<button onclick="l('BK', elv('dist'));">Backward</button>
<button onclick="l('FD', elv('dist'));">Forward</button><br />
Dist: <input type="number" id="dist" value="20" /> <br />
Ang: <input type="number" id="angle" value="90" /><br />
<button onclick="l('LT', elv('angle'));">Left</button>
<button onclick="l('RT', elv('angle'));">Right</button><br />
<hr />

<button onclick="l('PU');">Pen Up</button>
<button onclick="l('PD');">Pen Down</button>
<hr />

<button class="button30" onclick="l('!ST');">Stop</button>
<button class="button30" onclick="l('BZ', 500);">Horn</button>
<button class="button30" onclick="l('!SE');">E-Stop!</button>
<hr/>

X:<input style="width: 40px" type="number" id="x" value="0"/>
Y:<input style="width: 40px" type="number" id="y" value="0"/>
<br />
<button onclick="l('TO', elv('x'), elv('y'));">Move To</button>
<hr />


<button onclick="l('WT', elv('txt'));">Write text</button>
&nbsp;
<input type="text" id="txt" />
</div>

<script type="text/javascript">
    function el(id) { return document.getElementById(id); }
    function elv(id) { return el(id).value; }
    function l(cmd, p1, p2) {
        var xhReq = new XMLHttpRequest();
        var uri = '/cmd?action=' + cmd;
        if (p1) { uri += '&p1=' + encodeURIComponent(p1); }
        if (p2) { uri += '&p2=' + encodeURIComponent(p2); }
        xhReq.open('GET', uri, true);
  try {
          xhReq.send();
          xhReq.onload = function () {
            el('state').innerText = this.responseText;
          }
  } catch(ex) {
    el('state').innerText = 'send error';
  }
    };
</script>
</body>
</html>
)~";


static const char statusPage[] = R"~(
<!DOCTYPE html>
<meta name="viewport" content="width=device-width, initial-scale=1">
<html>
  <head>
    <title>Logobot Status</title>
    <link rel="stylesheet" type="text/css" href="style.css">
  </head>
<body>
<div>
  Status: <span id="s">?</span><br />
  X: <span id="x">0</span><br />
  Y: <span id="y">0</span><br />
  Ang: <span id="a">0</span><br />
  CmdQ: <span id="q">0</span><br />
</div>

<script type="text/javascript">
  function el(id) { return document.getElementById(id); }
  function elv(id) { return el(id).value; }
  function updateStatus() {
    var xhReq = new XMLHttpRequest();
    xhReq.open('GET', '/stat', true);
    try {
      xhReq.send();
      xhReq.onload = function () {
        var stat = JSON.parse(this.responseText);
        el('s').innerText = stat.response;
        el('x').innerText = stat.x;
        el('y').innerText = stat.y;
        el('a').innerText = stat.ang;
        el('q').innerText = stat.qSize;
        setTimeout(updateStatus, 1000);
      }
    } catch(ex) {
      el('s').innerText = 'err';
    }
  }

  setTimeout(updateStatus, 1000);
</script>
</body>
</html>

)~";


static const char batchPage[] = R"~(
<!DOCTYPE html>
<meta name="viewport" content="width=device-width, initial-scale=1">
<html>
  <head>
    <title>Logobot Commander</title>
    <link rel="stylesheet" type="text/css" href="style.css">
  </head>
  <body>
    <div>
      <textarea rows="10" id="cmds" placeholder="enter logo commands, one per line.">
FD 50
RT 90
PD
BK 10
LT 45
PU
BZ 500
      </textarea> <br />
      <button onclick="s();">Send</button><br />
      <span id="f"></span>
    </div>

    <script type="text/javascript">
      function el(id) { return document.getElementById(id); }
      function elv(id) { return el(id).value; }

      function s() {
        var input = elv('cmds').trim();
        if (!input) return;

        var cmds = input.split(/\r?\n/);
        var next = cmds.shift();
        if (!next) return;

        el('f').innerText = 'Send: ' + next;

        var nextCmd = next.split(' ');
        var uri = '/cmd?action=' + nextCmd[0];
        if (nextCmd.length > 1)
          uri += '&p1=' + encodeURIComponent(nextCmd[1]);
        if (nextCmd.length > 2)
          uri += '&p2=' + encodeURIComponent(nextCmd[2]);


        var xhReq = new XMLHttpRequest();
        xhReq.open('GET', uri, true);
        console.log(uri);
        xhReq.onload = function () {
          if (this.responseText.trim() == 'BUSY') {
            el('f').innerText = 'BUSY';
            setTimeout(s, 2000);
          } else {
            el('f').innerText = this.responseText;
            el('cmds').value = cmds.join('\n');
            setTimeout(s, 250);
          }
        };

        try {
          xhReq.send();
        } catch (ex) {
          el('f').innerText = 'ERR';
          setTimeout(s, 2000);
        }
      }
    </script>
  </body>
</html>
)~";

static const char drawPage[] = R"~(
<!DOCTYPE html>
<meta name="viewport" content="width=device-width, initial-scale=1">
<html>
  <head>
    <title>Logobot Sketchpad</title>
    <link rel="stylesheet" type="text/css" href="style.css">
    <style>
    html,body {
        height:100%;
    }
    canvas {
        border:1px solid #eee;
    }
    #cd {
        height:80%;
    }
    </style>
  </head>
  <body onload="ol();">
      <div id="cd"></div>
    <div>
      <button onclick="clr();">Clear</button>
      <button onclick="gen();s();">Send</button><br />
      <span id="f" class="header-fixed"></span>
    </div>

    <script type="text/javascript">
     var ctx;
     var pts = [];
     var cx=0,cy=0;
     var cmds = [];

      function ol() {
        var cd = document.getElementById('cd');
        c = document.createElement('canvas');
        c.setAttribute('width', cd.offsetWidth);
        c.setAttribute('height', cd.offsetHeight-10);
        c.setAttribute('id', 'canvas');
        cd.appendChild(c);
        ctx = c.getContext("2d");
        cx = c.width / 2;
        cy = c.height / 2;

        cd.addEventListener("mouseup", function(e) {
            pts.push([(e.pageX - cd.offsetLeft) - cx, cy - (e.clientY - cd.offsetTop)]);
            redraw();
            return false;
        });

        redraw();
      }

      function redraw() {
          ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
          ctx.strokeStyle = "#aaa";
          ctx.beginPath();
          ctx.moveTo(cx-20, cy);
          ctx.lineTo(cx+20, cy);
          ctx.moveTo(cx, cy-20);
          ctx.lineTo(cx, cy+20);
          ctx.stroke();
          ctx.closePath();

          ctx.lineWidth = 2;
          ctx.beginPath();
          var x=cx;
          var y=cy;
          ctx.moveTo(x,y);
          for (i=0;i<pts.length;i++) {
              if (i==1) {
                  ctx.stroke();
                  ctx.strokeStyle = "#f00";
                  ctx.beginPath();
                  ctx.moveTo(x,y);
              }
              x=cx + pts[i][0];
              y=cy - pts[i][1];
              ctx.lineTo(x,y);
          }
          ctx.stroke();
      }

      function clr() {
          pts = [];
          redraw();
      }

      function gen() {
          cmds = [];
          cmds.push("CS");
          for (i=0;i<pts.length;i++) {
              x=pts[i][1];
              y=-pts[i][0];
              cmds.push("TO "+x/2+" "+y/2);
              if (i==0) cmds.push("PD");
          }
          cmds.push("PU");
          cmds.push("TO 0 -1");
          cmds.push("TO 0 0");
      }


      function el(id) { return document.getElementById(id); }
      function elv(id) { return el(id).value; }

      function s() {
        if (cmds.length==0) return;

        var next = cmds.shift();
        if (!next) return;

        el('f').innerText = 'Send: ' + next;

        var nextCmd = next.split(' ');
        var uri = '/cmd?action=' + nextCmd[0];
        if (nextCmd.length > 1)
          uri += '&p1=' + encodeURIComponent(nextCmd[1]);
        if (nextCmd.length > 2)
          uri += '&p2=' + encodeURIComponent(nextCmd[2]);

        var xhReq = new XMLHttpRequest();
        xhReq.timeout = 1000;
        xhReq.open('GET', uri, true);
        console.log(uri);
        xhReq.onload = function () {
          if (this.responseText.trim() == 'BUSY') {
            el('f').innerText = 'BUSY';
            cmds.unshift(next);
            setTimeout(s, 2000);
          } else {
            el('f').innerText = this.responseText;
            setTimeout(s, 250);
          }
        };
        xhReq.ontimeout = function() {
          el('f').innerText = 'TIMEOUT';
          cmds.unshift(next);
          setTimeout(s, 250);
        };

        try {
          xhReq.send();
        } catch (ex) {
          el('f').innerText = 'ERR';
          setTimeout(s, 2000);
        }
      }


    </script>
  </body>
</html>
)~";


static const char spiroPage[] = R"~(
<!DOCTYPE html>
<meta name="viewport" content="width=device-width, initial-scale=1">
<html>
  <head>
    <title>Logobot Spirograph</title>
    <link rel="stylesheet" type="text/css" href="style.css">
    <style>
    html,body {height:100%;}
    canvas {border:1px solid #eee;}
    #cd {height:80%;}
    </style>
  </head>
  <body onload="ol();">
      <div id="cd"></div>
    <div>
      <button onclick="mk();">Random</button>
      <button onclick="gen();s();">Send</button><br />
      <div id="f" class="header-fixed"></div>
    </div>

    <script type="text/javascript">
     var ctx;
     var pts = [];
     var cx=0,cy=0;
     var cmds = [];
     var numCmds = 0;
     var bigR = 100;
     var r = 76;
     var d = 100;

     function sqr(a) { return a*a;}

      function ol() {
        var cd = document.getElementById('cd');
        c = document.createElement('canvas');
        c.setAttribute('width', cd.offsetWidth);
        c.setAttribute('height', cd.offsetHeight-10);
        c.setAttribute('id', 'canvas');
        cd.appendChild(c);
        ctx = c.getContext("2d");
        cx = c.width / 2;
        cy = c.height / 2;

        redraw();
      }

      function redraw() {
          pts = [];
          ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
          ctx.strokeStyle = "#aaa";
          ctx.beginPath();
          ctx.moveTo(cx-20, cy);
          ctx.lineTo(cx+20, cy);
          ctx.moveTo(cx, cy-20);
          ctx.lineTo(cx, cy+20);
          ctx.stroke();
          ctx.closePath();

          ctx.lineWidth = 2;
          ctx.beginPath();
          function r1(ang) { return ang * (bigR - r)/r  };
          function x(ang) { return (bigR - r)*Math.cos(ang) + d*Math.cos(r1(ang)) };
          function y(ang) { return (bigR - r)*Math.sin(ang) - d*Math.sin(r1(ang)) };
          var theta = 0;
          var cycles = 50;
          var steps = 15;
          var points = cycles * steps;
          var startx = x(theta);
          var starty = y(theta);
          var i=0;
          var lx=0,ly=0;
          ctx.moveTo(cx + startx,cy - starty);
          pts.push([startx,starty]);
          for (i=1;i<points;i++) {
              theta = 2 * Math.PI * i/steps;
              var x1 = x(theta);
              var y1 = y(theta);
              ctx.lineTo(cx + x1, cy - y1);
              if (sqr(x1- lx)+ sqr(y1-ly) > 1)
                pts.push([x1,y1]);
              if (sqr(x1- startx)+ sqr(starty-y1) < 1) {
                  break;
              }
              lx = x1; ly=y1;
          }ctx.stroke();
      }

      function mk() {
          bigR = Math.round(Math.random() * 200);
          var r = Math.round(Math.random() * 100);
          var d = Math.round(Math.random() * 100);
          redraw();
      }

      function gen() {
          cmds = [];
          cmds.push("CS");
          for (i=0;i<pts.length;i++) {
              x=pts[i][1];
              y=-pts[i][0];
              cmds.push("TO "+(x/2).toFixed(1)+" "+(y/2).toFixed(1));
              if (i==0) cmds.push("PD");
          }
          cmds.push("PU");
          cmds.push("TO 0 0");
          numCmds = cmds.length;
      }

      function el(id) { return document.getElementById(id); }
      function elv(id) { return el(id).value; }

      function s() {
        if (cmds.length==0) return;

        var next = cmds.shift();
        if (!next) return;

        el('f').innerText = 'Send: ' + next;

        var nextCmd = next.split(' ');
        var uri = '/cmd?action=' + nextCmd[0];
        if (nextCmd.length > 1)
          uri += '&p1=' + encodeURIComponent(nextCmd[1]);
        if (nextCmd.length > 2)
          uri += '&p2=' + encodeURIComponent(nextCmd[2]);

        var xhReq = new XMLHttpRequest();
        xhReq.timeout = 1000;
        xhReq.open('GET', uri, true);
        console.log(uri);
        xhReq.onload = function () {
          if (this.responseText.trim() == 'BUSY') {
            el('f').innerText = 'BUSY';
            cmds.unshift(next);
            setTimeout(s, 2000);
          } else {
            el('f').innerText = this.responseText + ' '+(100*(1-cmds.length/numCmds)).toFixed(0)+'%';
            setTimeout(s, 250);
          }
        };
        xhReq.ontimeout = function() {
          el('f').innerText = 'TIMEOUT';
          cmds.unshift(next);
          setTimeout(s, 250);
        };

        try {
          xhReq.send();
        } catch (ex) {
          el('f').innerText = 'ERR';
          setTimeout(s, 2000);
        }
      }


    </script>
  </body>
</html>
)~";

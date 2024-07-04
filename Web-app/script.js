/* To select which Operation page we are in */
const urlParametersParsed = new URLSearchParams(window.location.search);
const operation = urlParametersParsed.get("operation");

if (operation == "listdata" || operation == "controlcar") {
  const ulops = document.querySelector("#operations");
  ulops.remove();
} else {
  document
    .querySelector("body")
    .setAttribute(
      "style",
      "display: flex; flex-direction: column; align-items: center; justify-content: center; height: 90vh;"
    );

  const listdataBtn = document.querySelector("#listdata");
  const controlcarBtn = document.querySelector("#controlcar");

  listdataBtn.addEventListener(
    "click",
    () => (window.location.href = "./index.html?operation=listdata")
  );
  controlcarBtn.addEventListener(
    "click",
    () => (window.location.href = "./index.html?operation=controlcar")
  );
}

/* Importing the functions and objects from Initialize_firebase.js file to use with our inputs */
import {
  fetchData,
  directionRef,
  speedRef,
  update,
} from "./initialize_firebase.js";

let firebaseFetchedData;
fetchData((data) => {
  firebaseFetchedData = data;
  parseData(firebaseFetchedData);

  // const arrayOfValues[];
  // arrayOfValues[0] = data["Camera"]["camera"];
  console.log(data["Camera"]["camera"]);
});

function parseData(data) {
  const keys = [
    "Camera",
    "Directions",
    "IR_RIGHT",
    "UltraSonic",
    "Speed",
    "Timestamp",
    "Servo",
	"IR_LEFT",
	"IR_BACK",
  ];
  const values = [
    data["Camera"]["camera"],
    data["Directions"]["directions"],
    data["IR_RIGHT"],
    data["Servo"]["servo"],
    data["Speed"]["speed"],
    data["Timestamp"]["timestamp"],
    data["UltraSonic"]["ultrasonic"],
    data["IR_LEFT"],
    data["IR_BACK"]
  ];
  whichOperation(keys, values);
}

function whichOperation(keys, values) {
  const main = document.querySelector("main");
  if (operation == "listdata") {
    main.innerHTML = `	    
		    <article id="article">
		      <section>
			<h2>Car Data</h2>
			<div id="figure">
					<div id="IR_LEFT">
						<p><span class="bold">${keys[7]}</span><br>&nbsp;&nbsp;&nbsp;- Mapped_Reads: ${values[7]["Mapped_Reads"]} <br>&nbsp;&nbsp;&nbsp;- Real_Reads: ${values[7]["Real_Reads"]}</p>
					</div>
				  	<img src="${values[0]}" />
				  	<div id="IR_RIGHT">		
						<p><span class="bold">${keys[2]}</span><br>&nbsp;&nbsp;&nbsp;- Mapped_Reads: ${values[2]["Mapped_Reads"]} <br>&nbsp;&nbsp;&nbsp;- Real_Reads: ${values[2]["Real_Reads"]} <p/>
					</div>
				</div>
				<div id="IR_BACK">
					<p><span class="bold">${keys[8]}</span><br>&nbsp;&nbsp;&nbsp;- Mapped_Reads: ${values[8]["Mapped_Reads"]} <br>&nbsp;&nbsp;&nbsp;- Real_Reads: ${values[8]["Real_Reads"]}</p>
				</div>
				<hr id="data_items_separator">
				<div id="data_items">
					<p><span class="bold">${keys[1]}</span>: ${values[1]}</p>
					<p><span class="bold">${keys[3]}</span>: ${values[3]}</p>
					<p><span class="bold">${keys[4]}</span>: ${values[4]}</p>
					<p><span class="bold">${keys[6]}</span>: ${values[6]}</p>
					<p><span class="bold">${keys[5]}</span>: ${values[5]}</p>				
     			       </div>
		      </section>
			  <div class="radar-container">
			    <div class="radar" id="radar"></div>
			    <div class="needle" id="needle"></div>
			  </div>
		    </article>
		`;

		setRadarAngle(values[6]);
  } else if (operation == "controlcar") {
    main.innerHTML = `	    
			<article id="article">
		      <section id="dashboard">
				<h2>Control the Car</h2>
				<div id="figure">
					<div id="IR_LEFT">
						<p><span class="bold">${keys[7]}</span><br>&nbsp;&nbsp;&nbsp;- Mapped_Reads: ${values[7]["Mapped_Reads"]} <br>&nbsp;&nbsp;&nbsp;- Real_Reads: ${values[7]["Real_Reads"]}</p>
					</div>
				  	<img src="${values[0]}" />
				  	<div id="IR_RIGHT">		
						<p><span class="bold">${keys[2]}</span><br>&nbsp;&nbsp;&nbsp;- Mapped_Reads: ${values[2]["Mapped_Reads"]} <br>&nbsp;&nbsp;&nbsp;- Real_Reads: ${values[2]["Real_Reads"]} <p/>
					</div>
				</div>
				<div id="IR_BACK">
					<p><span class="bold">${keys[8]}</span><br>&nbsp;&nbsp;&nbsp;- Mapped_Reads: ${values[8]["Mapped_Reads"]} <br>&nbsp;&nbsp;&nbsp;- Real_Reads: ${values[8]["Real_Reads"]}</p>
				</div>
				<hr id="data_items_separator">
				<div id="data_items">
					<p><span class="bold">${keys[1]}</span>: ${values[1]}</p>
					<p><span class="bold">${keys[3]}</span>: ${values[3]}</p>
					<p><span class="bold">${keys[4]}</span>: ${values[4]}</p>
					<p><span class="bold">${keys[6]}</span>: ${values[6]}</p>
					<p><span class="bold">${keys[5]}</span>: ${values[5]}</p>
				</div>
		      </section>
		        <div class="content">
			    <svg id="to-bottom">
			      <polygon class="arrow-top" points="37.6,27.9 1.8,1.3 3.3,0 37.6,25.3 71.9,0 73.7,1.3 "/>
			      <polygon class="arrow-middle" points="37.6,45.8 0.8,18.7 4.4,16.4 37.6,41.2 71.2,16.4 74.5,18.7 "/>
			      <polygon class="arrow-bottom" points="37.6,64 0,36.1 5.1,32.8 37.6,56.8 70.4,32.8 75.5,36.1 "/>
			    </svg>
    
			    <svg id="to-top">
			      <polygon class="arrow-top" points="37.6,27.9 1.8,1.3 3.3,0 37.6,25.3 71.9,0 73.7,1.3 "/>
			      <polygon class="arrow-middle" points="37.6,45.8 0.8,18.7 4.4,16.4 37.6,41.2 71.2,16.4 74.5,18.7 "/>
			      <polygon class="arrow-bottom" points="37.6,64 0,36.1 5.1,32.8 37.6,56.8 70.4,32.8 75.5,36.1 "/>
			    </svg>
    
			    <svg id="to-right">
			      <polygon class="arrow-top" points="37.6,27.9 1.8,1.3 3.3,0 37.6,25.3 71.9,0 73.7,1.3 "/>
			      <polygon class="arrow-middle" points="37.6,45.8 0.8,18.7 4.4,16.4 37.6,41.2 71.2,16.4 74.5,18.7 "/>
			      <polygon class="arrow-bottom" points="37.6,64 0,36.1 5.1,32.8 37.6,56.8 70.4,32.8 75.5,36.1 "/>
			    </svg>
    
			    <svg id="to-left">
			      <polygon class="arrow-top" points="37.6,27.9 1.8,1.3 3.3,0 37.6,25.3 71.9,0 73.7,1.3 "/>
			      <polygon class="arrow-middle" points="37.6,45.8 0.8,18.7 4.4,16.4 37.6,41.2 71.2,16.4 74.5,18.7 "/>
			      <polygon class="arrow-bottom" points="37.6,64 0,36.1 5.1,32.8 37.6,56.8 70.4,32.8 75.5,36.1 "/>
			    </svg>
			  </div>	

			  <div class="radar-container">
			    <div class="radar" id="radar"></div>
			    <div class="needle" id="needle"></div>
			  </div>

			  <div id="speed_selector">
			  	<label for="speeds">Speed: </label>
				<select name="speeds" id="speeds">
				  <option value="1">1</option>
				  <option value="2">2</option>
				  <option value="3">3</option>
				  <option value="4">4</option>
				  <option value="5">5</option>
				  <option value="6">6</option>
				  <option value="7">7</option>
				  <option value="8">8</option>
				  <option value="9">9</option>
				  <option value="10">10</option>
				</select>
			  </div>
		    </article>
		`;

    setRadarAngle(values[6]);

    // The script of settings speed in Firebase
    const speed_selector = document.querySelector("#speeds");
    speed_selector.addEventListener("change", () => {
      sendSpeed(speed_selector.options[speed_selector.selectedIndex].value);
      document
        .querySelector(
          `option[value="${
            speed_selector.options[speed_selector.selectedIndex].value
          }"]`
        )
        .setAttribute("selected", "");
    });

    // The script to send char to Directions in firebase
    const svgLogMap = {
      "#to-bottom": 2,
      "#to-top": 8,
      "#to-right": 6,
      "#to-left": 4,
    };

    // Variable to hold the interval ID and the current character being logged
    let logInterval;
    let currentNum = 0;

    // Function to start logging
    function startLogging(numDir) {
      return setInterval(() => {
        console.log(numDir + "\n");
        sendDirection(numDir);
      }, 100); // Adjust the interval as needed
    }

    // Function to stop logging
    function stopLogging() {
      if (logInterval) {
        clearInterval(logInterval);
        logInterval = null;
        sendDirection(0);
      }
    }

    // Add event listeners to each SVG element
    Object.keys(svgLogMap).forEach((selector) => {
      const svg = document.querySelector(selector);
      const numDir = svgLogMap[selector];

      svg.addEventListener("mousedown", () => {
        // Start logging when mouse is pressed, only if not already logging the same character
        if (currentNum !== numDir) {
          stopLogging();
          logInterval = startLogging(numDir);
          currentDir = numDir;
        }
      });

      svg.addEventListener("mouseup", stopLogging);
      svg.addEventListener("mouseleave", stopLogging);
    });

    // Add event listener to stop logging and log 'S' if mouse is released anywhere
    document.addEventListener("mouseup", stopLogging);
  }
}

/* The script for radar component */
function setRadarAngle(angleValue) {
  let angle = parseInt(angleValue);

  if (isNaN(angle) || angle < 0 || angle > 180) {
    alert(
      `Please enter a valid angle between 0 and 180. \n Because the current value of angle is: ${angle}`
    );
    return;
  }

  needle.style.transform = `rotate(${angle - 90}deg)`;
}

function sendDirection(dirNum) {
  update(directionRef, { directions: dirNum }).catch((error) => {
    console.error("Error updating Direction value: ", error);
  });
}

function sendSpeed(speedValue) {
  update(speedRef, { speed: speedValue }).catch((error) => {
    console.error("Error updating Speed value: ", error);
  });
}

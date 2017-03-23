/*
 * Constants
 */
 const CELL_WIDTH = 50;
 const CELL_HEIGHT = 50;
 const TICK_RATE = 50; // in ms

 var cells = [];
 var canvas;
 var statusCanvas = document.createElement("canvas");
 var status = "Preparing...";

 // Cell class
 class Cell {
 	constructor(x, y) {
 		this.x = x;
 		this.y = y;
 		this.alive = false;
 		this.neighbors = [];
 		this.imageData = null;
 	}

 	getCoordinate() {
 		return { x: this.x, y: this.y }
 	}

 	drawCell(canvas) {
 		let context = canvas.getContext("2d");
 		context.moveTo(this.getCoordinate().x, this.getCoordinate().y);
 		context.lineTo(this.getCoordinate().x + 50, this.getCoordinate().y);
 		context.moveTo(this.getCoordinate().x, this.getCoordinate().y);
 		context.lineTo(this.getCoordinate().x, this.getCoordinate().y + 50);

 		context.strokeStyle = "black";
 		context.stroke();
 	}

 	drawState(canvas) {
 		let context = canvas.getContext("2d");
 		context.fillText(this.isAlive(), this.getCoordinate().x, this.getCoordinate().y);
 		context.stroke();
 	}

 	setCanvasAndState(canvas) {
 		let context = canvas.getContext("2d");
 		let imageData = context.getImageData(this.getCoordinate().x, this.getCoordinate().y, 50, 50);
		//let aliveArray = [];
		let aliveArraySample = [];
		for (let i = 0; i < (CELL_HEIGHT * CELL_WIDTH); i ++) {
			aliveArraySample.push(imageData.data.slice(i, i+4));
		}
		if (getMajorityColor(aliveArraySample) == "black") {
			this.alive = true;
		} else {
			this.alive = false;
		}
		this.imageData = imageData;
	}

	isAlive() {
		return this.alive;
	}

	addNeighbor(neighbor) {
		if (neighbor != undefined)
			this.neighbors.push(neighbor);
	}

	getNeighbors() {
		return this.neighbors.filter((n) => n.isAlive());
	}

	setImageData(data) {
		let context = canvas.getContext("2d");
		context.putImageData(data, this.x, this.y);
	}
}

// WIP
const updateStatus = (text) => {
	status = text;
	let context = statusCanvas.getContext("2d");
	statusCanvas.width = 100;
    statusCanvas.height = 100;
	context.clearRect(100, 100, 100, 100);
	context.font = "30px Arial";
	context.fillText(status, 100, 100);
}

// needs refactoring
const getCellAtCoordinate = (x, y) => {
	let tempCell = undefined;
	for (let cell of cells) {
		if (cell.getCoordinate().x === x && cell.getCoordinate().y === y) {
			tempCell = cell;
			break;
		}
	}
	return tempCell;
}

// needs refactoring
const getMajorityColor = (arr) => {
	let b = 0;
	let w = 0;
	arr.forEach((element) => {
		if (element[0] > 235 && element[1] > 235 && element[2] > 235)
			w++; // dead
		else
			b++; // alive
	});
	return b > w ? "black" : "white"
}

// destroys a cell by setting alive to false and setting opacity to 0.
const destroyCell = (cell) => {
	cell.alive = false;
	let data = cell.imageData;
	data[3] = 0;
	cell.setImageData(data);
}

// assign random image data from a neighbor to the cell specified
const getRandomImageDataFromNeighbor = (cell) => {
	cell.alive = true;
	neighbors = cell.getNeighbors();
	let randomNeighbor = neighbors[Math.floor(Math.random()*neighbors.length)];
	cell.setImageData(randomNeighbor.imageData);
}

// actual conways game of life rules
const calculateCellMove = (cell) => {
	let neighbors = cell.getNeighbors();
	if (cell.isAlive()) {
		if (neighbors.length >= 4 || neighbors.length <= 1) {
			destroyCell(cell)
		}
	} else {
		if (neighbors.length == 3) {
			getRandomImageDataFromNeighbor(cell);
		}
	}
}

// run game ticks
const runGame = () => {
	setInterval(() => {
		step();
		status = "Running";
		updateStatus(status);
		console.log("stepping");
	}, TICK_RATE);
}

// actual tick step
const step = () => {
	cells.forEach((cell) => {
		calculateCellMove(cell);
	});
}

// create canvas and start the game
const createSS = (event) => {
	console.log("LOADED");
	html2canvas(document.body, {
		onrendered: function(c) {
			canvas = c;
			createGrid(canvas);
			runGame();
		}
	});
};

// creates grid
const createGrid = (canvas) => {
		console.log("...drawing grid");
		updateStatus("Drawing Grid...");
		let bw = canvas.width;
		let bh = canvas.height;

		let context = canvas.getContext("2d");

		for (let x = 0; x <= bw; x += 50) {
			context.moveTo(x, 0);
			context.lineTo(x, bh);
		}

		for (let y = 0; y <= bh; y += 50) {
			context.moveTo(0, y);
			context.lineTo(bw, y);
		}

		context.stroke();

		console.log("...done drawing grid");
		//updateStatus("Creating Cells...");

		for (let x = 0; x < bw; x += CELL_WIDTH) {
			for (let y = 0; y < bh; y += CELL_HEIGHT) {
				let cell = new Cell(x, y);
				//cell.drawCell(canvas);
				cells.push(cell);
			}
		}

		console.log("...created cells");
		//updateStatus("Creating Neighbors...")

		// add surrounding neighbors
		for (let x = 0; x < bw; x += CELL_WIDTH) {
			for (let y = 0; y < bh; y += CELL_HEIGHT) {
				let cell = getCellAtCoordinate(x, y);
				cell.addNeighbor(getCellAtCoordinate(x-50, y-50))
				cell.addNeighbor(getCellAtCoordinate(x-50, y))
				cell.addNeighbor(getCellAtCoordinate(x, y-50))
				cell.addNeighbor(getCellAtCoordinate(x, y+50))
				cell.addNeighbor(getCellAtCoordinate(x+50, y))
				cell.addNeighbor(getCellAtCoordinate(x+50, y+50))
				cell.addNeighbor(getCellAtCoordinate(x+50, y-50))
				cell.addNeighbor(getCellAtCoordinate(x-50, y+50))
				cell.setCanvasAndState(canvas);
				//cell.drawState(canvas);
			}
		}

		console.log("...added neighbors to cells");

		// wipe body html and replace it with the canvas
		document.body.innerHTML = "";
		document.body.appendChild(canvas);
 };

/*                                                                *
 * ====================Possible Greyscaling?===================== *
 *                             (WIP)                              */

const getImageData = (canvas) => {
	var context = canvas.getContext("2d");
	var width = canvas.width;
	var height = canvas.height;
	var imageData = context.getImageData(0, 0, width, height);
	return imageData;
};

const greyScale = (canvas) => {
	var context = canvas.getContext("2d");
	var imageData = getImageData(canvas);
	var data = imageData.data;

	console.log("...greyscaling");
	for(var i = 0; i < data.length; i += 4) {
		var brightness = 0.34 * data[i] + 0.5 * data[i + 1] + 0.16 * data[i + 2];
          // red
          if (data[i] >= 225) {
          	data[i] = 255;
          } else if (data[i] < 225) {
          	data[i] = 0;
          } else {
          	data[i] = brightness;
          }
          // green
          if (data[i+1] >= 225) {
          	data[i+1] = 255;
          } else if (data[i+1] < 225) {
          	data[i+1] = 0;
          } else {
          	data[i+1] = brightness;
          }
          // blue
          if (data[i+2] >= 225) {
          	data[i+2] = 255;
          } else if (data[i+2] < 225) {
          	data[i+2] = 0;
          } else {
          	data[i+2] = brightness;
          }
      }

        // overwrite original image
        context.putImageData(imageData, 1, 1);
        console.log("...loaded greyscale");
        return canvas;
    };

// run function on execution of script
createSS();

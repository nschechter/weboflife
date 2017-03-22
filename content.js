/*
 * Constants
 */
 const CELL_WIDTH = 50;
 const CELL_HEIGHT = 50;

 var cells = [];

 class Cell {
 	constructor(x, y, alive) {
 		this.x = x;
 		this.y = y;
 		this.alive = false;
 	}

 	getCoordinate() {
 		return { x: this.x, y: this.y }
 	}

 	drawCell(canvas) {
 		let context = canvas.getContext("2d");
 		//context.fillText(this.x, this.getCoordinate().x, this.getCoordinate().y);
		context.moveTo(this.getCoordinate().x, this.getCoordinate().y);
		context.lineTo(this.getCoordinate().x + 50, this.getCoordinate().y);
		context.moveTo(this.getCoordinate().x, this.getCoordinate().y);
		context.lineTo(this.getCoordinate().x, this.getCoordinate().y + 50);

		context.strokeStyle = "black";
		context.stroke();
	}

	isAlive(canvas) {
		let context = canvas.getContext("2d");
		let imageData = context.getImageData(this.getCoordinate().x, this.getCoordinate().y, 50, 50);
		//let aliveArray = [];
		let aliveArraySample = [];
		for (let i = 0; i < (CELL_HEIGHT * CELL_WIDTH); i += 25) {
			aliveArraySample.push(imageData.data.slice(i, i+4));
		}
		if (getMajorityColor(aliveArraySample) == "white") {
			return false;
			this.alive = false;
		} else {
			return true;
			this.alive = true;
		}
	}

	getNeighbors() {
		temp = [];

		return 
	}
}

const destroyCell = (cell) => {
}

const getCellAtCoordinate = (x, y) => {
	let tempCell = undefined;
	cells.forEach((cell) => {
		if (cell.getCoordinate().x === x && cell.getCoordinate().y === y) {
			tempCell = cell;
			break;
		}
	})
	return tempCell;
}

const getMajorityColor = (arr) => {
	let b = 0;
	let w = 0;
	arr.forEach((element) => {
		if (element[0] == 255)
			b++;
		else
			w++;
	});
	return w > b ? "white" : "black"
}

const calculateCellMove = (cell) => {
	if (cell.isAlive()) {
		let neighbors = getNeighbors();
		if (cell.neighbors.length > 3) {

		} else if (cell.neighbors.length <= 1) {
			destroyCell(cell);
		}
	}
}

const runGame = () => {
	setInterval(() => {
		step();
		console.log("stepping");
	}, 3000);
}

const step = () => {
	cells.forEach((cell) => {
		calculateCellMove(cell);
	});
}

const createSS = (event) => {
	console.log("LOADED");
	html2canvas(document.body, {
		onrendered: function(canvas) {
			createGrid(greyScale(canvas));
			runGame();
		}
	});
};

// creates grid
const createGrid = (canvas) => {
		//grid width and height
		var bw = canvas.width;
		var bh = canvas.height;

		//padding around grid (set to 0 for now)
		var p = 0;
		//size of canvas
		var cw = bw + (p*2) + 1;
		var ch = bh + (p*2) + 1;

		var context = canvas.getContext("2d");
		//(bw / 50) * (bh / 50)
		console.log("...drawing grid");

		for (let x = 0; x < bw; x += CELL_WIDTH) {
			for (let y = 0; y < bh; y += CELL_HEIGHT) {
				let cell = new Cell(x, y);
				cell.drawCell(canvas);
				cells.push(cell);
				cell.isAlive(canvas);
			}
		}
		console.log("...loaded cells");
		document.body.innerHTML = "";
		document.body.appendChild(canvas);
	};

// returns image data for the entire canvas
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
          if (data[i] > 245) {
          	data[i] = 255;
          } else if (data[i] < 10) {
          	data[i] = 0;
          } else {
          	data[i] = brightness;
          }
          // green
          if (data[i+1] > 245) {
          	data[i+1] = 255;
          } else if (data[i+1] < 10) {
          	data[i+1] = 0;
          } else {
          	data[i+1] = brightness;
          }
          // blue
          if (data[i+2] > 245) {
          	data[i+2] = 255;
          } else if (data[i+2] < 10) {
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

    window.addEventListener ("load", createSS, false);

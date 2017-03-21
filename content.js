class Cell {
	constructor(x, y) {
		this.x = x;
		this.y = y;
	}

	getCoordinate() {
		return { x: this.x, y: this.y }
	}

	drawCell(canvas) {
		let context = canvas.getContext("2d");

		// x line
		context.moveTo(this.getCoordinate().x, this.getCoordinate().y);
		context.fillText(this.x, this.getCoordinate().x, this.getCoordinate().y);
	    context.lineTo(this.getCoordinate().x + 50, this.getCoordinate().y);
	    context.moveTo(this.getCoordinate().x, this.getCoordinate().y);
	    context.lineTo(this.getCoordinate().x, this.getCoordinate().y + 50);

	    // y line
	    //context.moveTo(0, 0.5 + this.getCoordinate().y);
		//context.lineTo(this.getCoordinate().y, 0.5 + this.getCoordinate().y);

	    context.strokeStyle = "black";
	    context.stroke();
	}
}

var cells = [];

const createSS = function(event) {
	console.log("LOADED");
	html2canvas(document.body, {
		onrendered: function(canvas) {
			// canvas.width = document.documentElement.clientWidth;
			// canvas.height = document.documentElement.clientHeight;
			// document.body.appendChild(canvas);
			createGrid(greyScale(canvas));
		}
	});
};

const getImageData = function (canvas) {
	var context = canvas.getContext("2d");
	var width = canvas.width;
	var height = canvas.height;
	var imageData = context.getImageData(0, 0, width, height);
	return imageData;
}

	const createGrid = function(canvas) {
        //document.body.appendChild(canvas);
		//grid width and height
		var bw = canvas.width;
		var bh = canvas.height;

		//padding around grid
		var p = 0;
		//size of canvas
		var cw = bw + (p*2) + 1;
		var ch = bh + (p*2) + 1;
		var context = canvas.getContext("2d");
		//(bw / 50) * (bh / 50)

		// rendering a line from 0 to canvas width
		// for (let x = 0; x <= bw; x += 50) {
	        // context.moveTo(0.5 + x + p, p);
	        // context.lineTo(0.5 + x + p, bh + p);
		// }

		// rendering a line from 0 to canvas height
		// increment 50 each time
		// for (let y = 0; y <= bh; y += 50) {
		    // context.moveTo(p, 0.5 + y + p);
		    // context.lineTo(bw + p, 0.5 + y + p);
		// }

		// for (let i = 0; i < ((bw / 50) * (bh / 50)); i++) {
		// 	// x
		// 	context.moveTo(0.5 + x + p, p);
	 //        context.lineTo(0.5 + x + p, bh + p);

	 //        // y
		// 	context.moveTo(p, 0.5 + y + p);
		//     context.lineTo(bw + p, 0.5 + y + p);
		// }

		for (let x = 0; x < bw; x += 50) {
			for (let y = 0; y < bh; y += 50) {
				let cell = new Cell(x, y);
				cell.drawCell(canvas);
				cells.push(cell);
			}
		}
	    console.log("drawing grid");
		document.body.innerHTML = "";
		document.body.appendChild(canvas);
    }

    const greyScale = function(canvas) {
    	var context = canvas.getContext("2d");
    	var imageData = getImageData(canvas);
    	var data = imageData.data;

    	for(var i = 0; i < data.length; i += 4) {
    		var brightness = 0.34 * data[i] + 0.5 * data[i + 1] + 0.16 * data[i + 2];
          // red
          data[i] = brightness;
          // green
          data[i + 1] = brightness;
          // blue
          data[i + 2] = brightness;
      }

        // overwrite original image
        context.putImageData(imageData, 1, 1);
        return canvas;
    }

    window.addEventListener ("load", createSS, false);

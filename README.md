# SSU Pond Area

This is code for an Arduino setup that measures the area of any arbitrarily-shaped pond for Sonoma State University.

The Arduino has two components: a potentiometer used to measure rotation angle and a rotary encoder to measure the distance of how far a string has been pulled from the setup.

The user positions the setup along the bank of a pond, grabs the string, and walks around the edge of the pond. During the walk, the Arduino is recording the angle and distance measurements (as raw sensor values) as a list of pairs and creates a CSV file output through the serial interface.

Mathematica is used to process the raw data into real-world measurements, turning sensor ticks into meters and angles to form a list of polar coordinates (distance and angles). The polar coordinates are turned into cartesian coordinates and interpreted as the ordered set of vertices of a polygon. The area of the polygon is then found with Mathematica.

# Contributors

* Cody Lynch (lynchco AT sonoma.edu)
* Hunter Fernandes (hfernandes.com)

# Usage

(Just once) make sure you've compiled the go serial2csv program, which reads the serial port from the Arduino and creates a csv file. You'll need to get either a binary or to compile it with the go toolchain.

Make sure that program is loaded onto Arduino board. Set-down the setup near the bond, plug into computer, double click serial2csv program and start collecting. When done, close serial2csv window. 

A .csv file should be on your desktop. Open Mathematica notebook and select the new .csv file. Refresh neccesary cells. :tada::confetti_ball:

Don't forget to save :floppy_disk: your results!


# License
MIT. See the LICENSE file.

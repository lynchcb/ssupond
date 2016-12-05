# SSU Pond Area

This is code for an Arduino setup that measures the area of any arbitrarily-shaped pond for Sonoma State University.

The Arduino has two components: a potentiometer used to measure rotation angle and a rotary encoder to measure the distance of how far a string has been pulled from the setup.

The user positions the setup along the bank of a pond, grabs the string, and walks around the edge of the pond. During the walk, the Arduino is recording the angle and distance measurements (as raw sensor values) as a list of pairs and creates a CSV file output through the serial interface.

Mathematica is used to process the raw data into real-world measurements, turning sensor ticks into meters and angles to form a list of polar coordinates (distance and angles). The polar coordinates are turned into cartesian coordinates and interpreted as the ordered set of vertices of a polygon. The area of the polygon is then found with Mathematica.

# Contributors

* Cody Lynch (lynchco AT sonoma.edu)
* Hunter Fernandes (hfernandes.com)

# Usage

__Todo!__

# License
MIT. See the LICENSE file.

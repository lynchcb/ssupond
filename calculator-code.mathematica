(*
Copyright 2017 Hunter Fernandes <hgfern (@) gmail.com>, Cody Lynch \
(lynchco (@) sonoma.edu)

	Permission is hereby granted,free of charge,to any person obtaining \
a copy of this software and associated documentation files (the \
"Software"),to deal in the Software without restriction,including \
without limitation the rights to \
use,copy,modify,merge,publish,distribute,sublicense,and/or sell \
copies of the Software,and to permit persons to whom the Software is \
furnished to do so,subject to the following conditions:

The above copyright notice and this permission notice shall be \
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS",WITHOUT WARRANTY OF ANY KIND,EXPRESS \
OR IMPLIED,INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF \
MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND \
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE \
LIABLE FOR ANY CLAIM,DAMAGES OR OTHER LIABILITY,WHETHER IN AN ACTION \
OF CONTRACT,TORT OR OTHERWISE,ARISING FROM,OUT OF OR IN CONNECTION \
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*)

LimitAngle[\[Theta]in_] := 
  If[# > Pi, # - 2*Pi, #] &[# &[Mod[\[Theta]in, 2*Pi]]];
Potent2Angle[
   potentiometerReading_] := (24.0*Log10[1 + potentiometerReading] + 
     7.1*Sqrt[potentiometerReading] ) Degree;
RotaryTicks2Meters[rotaryCount_] := 
  rotaryCount*Quantity[0.3048, "Meter"] / 40;
delimeter = "########## NEW SECTION ##########";
TimeTaken[pondData_] := Last[pondData][[3]] - First[pondData][[3]];
Graphics`Mesh`MeshInit[];


CalculatePondStats[pondData_, pondNo_] := Module[{samplePairs},
  samplePairs = Take[#, 2] & /@ pondData;
  lengthAnglePairs = {RotaryTicks2Meters[#1], 
      LimitAngle[Potent2Angle[#2]]} & @@@ samplePairs;
  cartesionPairs = 
   CoordinateTransform["Polar" -> "Cartesian", #1] & /@ 
    lengthAnglePairs;
  pointPlot = 
   ListPlot[cartesionPairs, 
    PlotLabel -> "Sensor Pairs Interreted on Cartesian"];
  polyPoints = QuantityMagnitude /@ cartesionPairs; 
  poly = Polygon[polyPoints];
  cartPlot = 
   Graphics[{LightBlue, EdgeForm[{Thin, Brown}], poly}, 
    PlotLabel -> "Shape of Pond", GridLines -> Automatic, 
    Axes -> True, ImageSize -> Large];
  polyArea = 
   Abs[PolygonArea[polyPoints]] * 
    QuantityUnit[First[First[cartesionPairs]]]^2;
  Grid[{{Grid[{{Text["The area of the pond is"], 
        Text[polyArea]}}]}, {Show[cartPlot, pointPlot]}}]
  ]

showDataFor[pondNo_] := 
  Grid[{{Style[
      Text["Pond No. " <> ToString[pondNo]], {Larger, Orange, 
       Bold}]}, {CalculatePondStats[ pondsData[[pondNo]], pondNo]}}, 
   Frame -> Outer];

ShowPondsInFile[filename_] := Module[{fileContents, ponds},
  fileContents = ReadList[fname, String];
  ponds = 
   DeleteCases[{}][
    DeleteCases[delimeter] /@ Split[fileContents, # != delimeter &]];
  pondsDataStrings = StringSplit[#, ","] & /@ (Drop[#, 1] & /@ ponds);
  pondsData = ({ToExpression[#[[1]]], ToExpression[#[[2]]], 
         ToExpression[#[[3]]]} & /@ #) & /@ pondsDataStrings;
  pondsHuman = {#, Length[pondsData[[#]]], 
      Round[TimeTaken[pondsData[[#]]]/1000], 
      Button["Calculate", Print[showDataFor[#]]]} & /@ 
    Table[n, {n, 1, Length[pondsData]}];
  Grid[Join[{{"Pond No.", "Data Samples", 
      "Approx Collection Time (s)"}}, pondsHuman], 
   Frame -> {{2 -> True, 1 -> True}, All}]
  ]

fname = ""; fpicker := 
 FileNameSetter[Dynamic[fname], 
  "Open", {"SSU Ponds Files" -> {"*.PD"}}];

Dynamic[If [fname != "", ShowPondsInFile[fname], 
  Grid[{{Style[Text["Error: Please select a Pond File: "], 
      FontColor -> Red, FontWeight -> Bold], fpicker}}]]]


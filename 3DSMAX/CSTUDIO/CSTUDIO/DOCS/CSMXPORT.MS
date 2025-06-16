-- A script to output a .csm file
-- Written by Adam Felt

-- This script assumes that you have set up a .max file containing 
-- a biped or bone structure with a set of markers linked to the 
-- skeleton.
-- The markers you want to record should be contained in a 
-- Selection Set titled "markers"
-- The script will output the data at every frame for the entire 
-- animation range.

-- The script begins here
-- *****************************************************************
fname = getSaveFileName caption:"Save Marker File" 
scalefacor = 25.4

if fname != undefined then
(
	f = createfile fname
	progressStart "Exporting Marker File"
	
	max time end
	endtime = currenttime
	max time start
	
	format "$comments" to:f
	format "\n" to:f
	
	comment = "Character Studio Marker file exported from 3D Studio MAX"
	format comment to:f
	format "\n\n" to:f
	
	format "$rate" to:f
	format "\n" to:f
	rate = framerate
	print rate to:f
	format "\n" to:f
 
	format "$order" to:f
	format "\n" to:f
   
	count = 1
	
	if(selectionSets["markers"] != undefined) then
	(
		for o in selectionSets["markers"] do
	    (
			if count == selectionSets["markers"].count then
				format "%" o.name to:f
			else
				format "%\t" o.name to:f
				
			count = count + 1
	    )
		format "\n\n" to:f
		format "$points" to:f
		format "\n" to:f

		endtime = endtime as float / 160
		for t in 0f to endtime by 1f do
		(
			sliderTime = t
			ct = (currenttime/160) as float 
			timeV = ct/30.0f 
		
			if (progressUpdate (ct*100/endtime as integer)) == FALSE then exit
			
			ct = (ct as integer) + 1
			format "%\t" ct  to:f
			
			count2 = 1
			for o in selectionSets["markers"] do
	   		(
				xpos = o.pos.x*scalefacor
				ypos = o.pos.y*scalefacor
				zpos = o.pos.z*scalefacor
				
				format "%\t" xpos to:f
	       		format "%\t" ypos to:f
				if count2 == selectionSets["markers"].count then
					format "%" zpos to:f
				else
					format "%\t" zpos to:f
					
				count2 = count2 + 1			
			)
			format "\n" to:f
		)
	)
	else messageBox "You must first create a Selection Set named\n\"markers\" that contains all your markers" title:"Incorrect Scene Setup"
	
	close f
	progressEnd()
)
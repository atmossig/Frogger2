	
	
-- 3/99 Fixed frame-off-by-1 bug by making keyframes start at 0 instead of 1, to match biped
-- 3/99 Fixed frame rate interpretation - it was dropping mocap data correctly, but not skipping max frames

	utility uReadCSM "Read CSM" 
(
	local HelperObj = Dummy

	fn strToArray str delims  = 
	(
	local i = 1
	local temp = "" 
	local arr = #()
	--	format "\t\t\tstr is \"%\"\n" str
		for i=1 to str.count do 
		(
	--	format "\t\t\ti=%\n" i
	--	format "\t\t\tstr[i]=%\n" str[i]
		
			if (findItem delims str[i]) > 0 then 
				(                               
				if temp != "" then append arr temp
				temp = ""
				)                       
			else temp += str[i]
			
	--	format "\t\t\tfinal temp = \"%\"\n" temp
	--	format "\t\t\tfinal arr = \"%\"\n" arr
		)
	if temp != "" then append arr temp
	arr
	)

	
	DropDownList 	dd_type		"Helper Object Type:"	items:#("Dummy", "Point")
	label			lbl_file	"                       "					align:#left
	label			lbl_rate	"                       " 			align:#left
	label			lbl_frame	"                        "				align:#left
	button 			btnRead 	"Read CSM..."
	CheckBox	dd_markerfilter "Use only specified markers"
	edittext 	marker_text "Markers:" text:""
	button 		btnget_markers 	"Load Marker Names..."

-- ------------------------------------------------------------------
-- get marker names from a user-specified CSM file and pre-load them
-- into the marker_text field
-- ------------------------------------------------------------------

	on btnget_markers pressed do
	(
	local markers = #() 
	local words = #()
	local key = #()
	local k = ""
	local marker_string = ""
	local cur_word = 1
	local filename = getOpenFilename caption:"Load Marker Names from CSM file..."
	local txt_ERROR = false
	local markers_found = false
	
	if filename == undefined then
		return false
		
	local fileptr = openFile filename

	lbl_file.caption = "Reading: " + (GetFileNameFile filename) + (GetFileNameType filename)
	
			do
			(
				k = readDelimitedString fileptr "\n"  -- read the next line in the file
				
				if (k.count > 0 ) then  -- only process non-blank lines
				(	
					-- format "processing line \"%\" with size %\n" k k.count
										
					key = (strToArray k #(" ", "\t"))				
					
					 -- format "key is \"%\"\n" key[1]
					 -- format "key count is %\n" key.count
					
					case of
					(
										(findItem #("$Order", "$order") key[1] > 0 ):
											(
												-- format "\tProcessing marker order section...\n" -- to:outfile
								
												do -- processing the order block
												(
													k = readDelimitedString fileptr "\n"
													if ((k.count != 0) and (k[1]!="$")) then 
													(
														-- format "\t\tstring is %\n" k
														-- format "\t\t\t size is %\n" k.count
														marker_string = k
														join markers (strToArray k #(" ", "\t"))
														-- format "\t\t\tmarkers are \"%\"\n" markers
													)
													
												) while (not eof fileptr) and (k[1] != "$")
												
							-- format "marker_string is %\n" marker_string
						 	marker_text.text = marker_string
												markers_found = true	
											)
											
						default:  -- do nothing
						(
						)  -- end of default case
						
					) -- end of case expr
					
				)  -- end of check for non-blank lines

			) While ((not eof fileptr) and (not markers_found)) 
			
        close fileptr
	)
	

	
    on dd_type selected sel do 
	(
		HelperObj = Execute (dd_type.selected) -- == "Dummy" then  Dummy else
	)

------------------------------------------------------
-- main routine: parse the csm file and create dummy/point objects
------------------------------------------------------

    on btnRead pressed do 
	(
	 local markers = #(), created = false, frate = 60
	 local skip_frames=0, skipped=0
	 
	local words = #()
	local key = #()
	local frame = ""
	local cur_word = 1
	local vtx_count = 0
	local k = ""
	local framecount = 0
	
	local markers = #() 			-- array of marker names contained in the csm file
	local markers_helpers = #()		-- array of helper objects for each marker
	local markers_parsed = false	-- true if/when we have successfully parsed the markers information
	local markers_filter = #()		-- subset of user-specified marker names that we will export to csm

	if dd_markerfilter.checked then -- the user has specified a subset of marker names to be converted
			markers_filter = strToArray marker_text.text #(" ", "\t", ",") -- build an array from the text field
	 
	local filename = getOpenFilename caption:"Read CSM File"
	if filename == undefined then return false			
		fileptr = openFile filename
		
--	outfile = openFile "csmlog.txt" mode:"w"
		
	lbl_file.caption = "Reading File: " + (GetFileNameFile filename) + (GetFileNameType filename) + "       "

			format "\n\n************************\n" -- to:outfile
			format "Processing file %\n" filename -- to:outfile
			
			
			k = readDelimitedString fileptr "\n"
			
			
			do 
			(
			
			-- skip garbage until we hit the first block
			
			if ((k == "") or (k[1] != "$"))  then -- each garbage
				(
--				format "skipping leading garbage... \"%\"\n" k
				k = readDelimitedString fileptr "\n"
				)
				else -- process valid data block
				(

					key = (strToArray k #(" ", "\t"))
					
--					format "found block \"%\" with size %\n" k k.count
--					format "key is \"%\"\n" key[1]
					
					case of
					(
					(findItem #("$Order", "$order") key[1] > 0 ):
						(
							format "\tProcessing marker order section...\n" -- to:outfile
							
							do -- processing the order block
							(
								k = readDelimitedString fileptr "\n"
								if ((k.count != 0) and (k[1]!="$")) then 
								(
--									format "\t\tstring is %\n" k
--									format "\t\t\t size is %\n" k.count
									join markers (strToArray k #(" ", "\t"))
--									format "\t\t\tmarkers are \"%\"\n" markers
									
									-- format "\t\t\tMaking dummy objects for the following markers...\n" -- to:outfile
									-- format "\t\t\t\t" -- to:outfile
									
									for i=1 to markers.count do
									(
									-- format "marker name before creation is %\n" markers[i]

										if dd_markerfilter.checked then -- the user has specified a subset of marker names to be converted
										(
											if (findItem markers_filter markers[i] > 0) then -- build helpers for matches only
													markers_helpers[i] = HelperObj name:(markers[i])
										)
											
										else -- create a helper for every marker name found
										(
												markers_helpers[i] = HelperObj name:(markers[i])
										)
									)
									
									-- format "\n"	 -- to:outfile	
									-- format "markers is %\n" markers
									-- format "markers_helpers is %\n" markers_helpers
	
								)
							) while (not eof fileptr) and (k[1] != "$")
						)
						
					(findItem #("$Points", "$points") key[1]  > 0 ):
						(
							format "\tProcessing points section...\n" -- to:outfile
							local tStart=timeStamp()			 
							
							do -- processing the points block
							(
								-- get a single line up to the "\n" symbol
								
								k = readDelimitedString fileptr "\n"
								
--								format "\n\t\tpoint line is \"%\"\n" k -- to:outfile
--								format "\t\tsize of point line is %\n" k.count -- to:outfile
								
								if ((k.count != 0) and (k[1]!="$")) then 
								(
								
--									format "\t\textracting words now...\n" -- to:outfile
									words = (strToArray k #(" ", "\t"))
									
--								    format "\t\t words are \"%\"\n" words  -- to:outfile
									
									frame = words[1]
	
									lbl_frame.caption = "Frame:" + frame	+ "        "			
								
									format "\t\tprocessing frame %...\n" frame -- to:outfile
									
--									format "\t\tWord count is %\n" words.count -- to:outfile
									
									vtx_count = 1
									cur_word =2
									
									if skipped == skip_frames then -- no need to skip this time
									(
	
										do
											(
--											format "\t\t\ti=%\n" cur_word -- to:outfile
											
--											format "words[i] = \"%\"\n" words[cur_word] -- to:outfile
											
											if (words[cur_word] == "DROPOUT") then
												(
--												format "found DROPOUT: \"%\"\n" words[cur_word] -- to:outfile
												cur_word += 1
												vtx_count = vtx_count + 1
												)
											
											else if (words[cur_word] == "" or words[cur_word] == "	" ) then
												(
--												format "found an extra blank space\n"
												cur_word += 1
												)	
											else
												(
												-- format"\t\t\txyz is \"% % %\"\n" words[cur_word] words[cur_word+1] words[cur_word+2] -- to:outfile			

												if dd_markerfilter.checked then -- the user has specified a subset of marker names to be read
													(
													if (findItem markers_filter markers[vtx_count] > 0) then -- read matches only
														(
								  						local pkey = addNewKey markers_helpers[vtx_count].pos.controller (framecount as integer)
												
								  						local x = words[cur_word] as float
								  						local y = words[cur_word+1] as float
								  						local z = words[cur_word+2] as float
								  
														pkey.value = [x, y, z]/25.4 	-- Convert millimeters to inches	
														)			
													)
							
											else -- process every marker name found
													(
								  					local pkey = addNewKey markers_helpers[vtx_count].pos.controller (framecount as integer)
												
								  					local x = words[cur_word] as float
								  					local y = words[cur_word+1] as float
								  					local z = words[cur_word+2] as float
								  
													pkey.value = [x, y, z]/25.4 	-- Convert millimeters to inches				
													)
													
												cur_word += 3
												vtx_count = vtx_count + 1
												)
												
											) 
										while cur_word <= words.count
											
										skipped = 0
--										format "\t\t\tfound % vertices in that line\n" (vtx_count-1) -- to:outfile
										framecount += 1
									)
										
					  	 			else 
										(
--										format "\t\t\tskipping a frame of mocap data\n" -- to:outfile
										skipped += 1    -- Increment "skipped" after each skip
										)
															
								) -- end if not blank line
								
								
							) while (not eof fileptr) and (k[1] != "$")
						)
						
					(findItem #("$Actor", "$actor") key[1]  > 0 ):
						(
							format "\tReading actor section...\n" -- to:outfile
							
							do -- processing the actor block
							(
								k = readDelimitedString fileptr "\n"
							) while (not eof fileptr) and (k[1] != "$")
						)
						
					(findItem #("$Time", "$time") key[1] > 0 ) :
						(
							format "\tReading time section...\n" -- to:outfile							
							do -- processing the time block
							(
								k = readDelimitedString fileptr "\n"
							) while (not eof fileptr) and (k[1] != "$")
						)
						
					
					(findItem #("$Date", "$date") key[1] > 0 ):
						(
							format "\tReading date section...\n" -- to:outfile
							
							do -- processing the date block
							(
								k = readDelimitedString fileptr "\n"
							) while (not eof fileptr) and (k[1] != "$")
						)
						
					(findItem #("$Filename", "$filename") key[1] > 0 ):
						(
							format "\tReading filename section...\n" -- to:outfile
							
							do -- processing the filename block
							(
								k = readDelimitedString fileptr "\n"
							) while (not eof fileptr) and (k[1] != "$")
						)
						
					(findItem #("$Comments", "$comments") key[1] > 0) :
						(
							format "\tSkipping comments section...\n" -- to:outfile
							
							do -- processing the comments block
							(
								k = readDelimitedString fileptr "\n"
							) while (not eof fileptr) and (k[1] != "$")
						)
						
					(findItem #("$Rate", "$rate") key[1] > 0 ):
						(
							format "\tProcessing rate section...\n" -- to:outfile
							words = (strToArray k #(" ", "\t"))
							
--							format "words is \"%\"\n" words
--							format "words count is %\n" words.count
							
							if (words.count == 2 and words[2] != "") then -- rate value is on this line
							(
--							format "found rate value on $rate line\n"
								frate = words[2] as integer
--								format "\tFrame rate is %\n" frate
							)
							
							else  -- rate value is not on this line
							(
								do -- eat blanks
								(
									k = readDelimitedString fileptr "\n"
								) while (not eof fileptr) and (k == "")
								
--								format "got a new line \"%\"\n" k
								
								if (not eof fileptr) then -- look for a rate value on this line
								(
--								format "fount a new value on the next line\n"
									words = (strToArray k #(" ", "\t"))
									if (words.count > 0 and words[1] != "") then
									(
										frate = words[1] as integer
									)
								)

							)


							
--							format "\twords[2] = %\n" words[2] -- to:outfile
							
							skip_frames = ((frate as float)/(frameRate as float)- 1.0 + 0.5) as integer
--							format "\t\tSkip_frames = %\n" skip_frames -- to:outfile
							skipped = skip_frames
							lbl_rate.caption = "Frame Rate:" + (frate as string)

							do -- processing rate block
							(
								k = readDelimitedString fileptr "\n"
							) while (not eof fileptr) and (k[1] != "$")
							
						)
						
					default:
						(
							format "\tSkipping unknown setion \"%\"...\n" k -- to:outfile
							
							do -- processing unknown block
							(
								k = readDelimitedString fileptr "\n"
							) while (not eof fileptr) and (k[1] != "$")
						)
			
						
					) -- end of case
					
				) -- end of block parsing
			)
			while not eof fileptr -- end of file parsing
			
        close fileptr
		format "\n************** DONE **********************\n" -- to:outfile
--		close outfile

	total_frames = (framecount) as string
	lbl_frame.caption = "Done (" + total_frames + " Frames Total)"		
	messageBox "The CSM file has been successfully read" title:"CSM read status"

		)
)
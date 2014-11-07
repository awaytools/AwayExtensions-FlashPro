
function documentInfos() {
	var doc = fl.getDocumentDOM(); 
	var collectallMotionsXML="All Motions";
	var animCounts=0;
	for(var i=0; i<doc.timelines.length;i++){
		var my_tl = doc.timelines[i]; 
		var countNum = my_tl.frameCount;
		fl.trace(countNum); 
		for(var l=0;l<my_tl.layerCount;l++){
			var my_ly =my_tl.layers[l];
			var fcountNum =my_ly.frameCount;
			//fl.trace(fcountNum); 
			//fl.trace(my_ly.frames.length); 
			//fl.trace("Layer-Animation = "+my_ly.animationType); 
			if(my_ly.animationType=="none"){// this still can be classic tween
				for(var t=0;t<fcountNum;t++){
					var my_tw=my_ly.frames[t];
					if(my_tw.tweenType=="motion"){
						if(my_tw.duration>1){
							fl.trace("tween-type = "+my_tw.tweenType);
							//if(!doc.timelines[i].layers[l].frames[t].isEmpty){ 
							if(my_tw.elements.length==1){
									var waslocked=my_ly.locked;
									my_ly.locked = false;
									my_tl.setSelectedLayers(l);
									
									my_tl.setSelectedFrames(t, t+my_tw.duration); // 'f' is the frame number, start & end
									var file = fl.configURI + 'Javascript/MotionXML.jsfl';
									
									var newMotion=fl.runScript(file, 'copyMotion');
									collectallMotionsXML+="#$TWEEN$#CLASSICTWEEN TimeLine "+my_tl.name+" LayerIdx "+l+" frame "+t+ " duration "+my_tw.duration+"\n";
									//fl.trace(newMotion);
									collectallMotionsXML+=newMotion+"\n";
									animCounts++;
									my_ly.locked = waslocked;
							}
						}
					}
					if(my_tw.tweenType=="shape"){
						fl.trace("found shape");
					}
					else{
						// no tween found						
					}
						
					t+=my_tw.duration-1;
				}
				
			}
			if(my_ly.animationType=="motion object"){
				
				for(var t=0;t<fcountNum;t++){
					var my_tw=my_ly.frames[t];
					//fl.trace(my_tw.duration);
					//fl.trace(my_tw.hasMotionPath());
					if(my_tw.isMotionObject()){
						fl.trace("fcountNum = "+fcountNum+" t="+t);		
						collectallMotionsXML+="#$TWEEN$#MOTIONOBJECT TimeLine "+my_tl.name+" LayerIdx "+l+" frame "+t+ " duration "+my_tw.duration+"\n";
						//fl.trace(newMotion);
						motionXML=doc.timelines[i].layers[l].frames[t].getMotionObjectXML();
						//fl.trace("nodes num: " + motionXML.PropertyContainer.length());
						collectallMotionsXML+=motionXML+"\n";					
						//fl.trace("node 2 name: " + motionXML.;
						animCounts++;
									
					}
					t+=my_tw.duration-1;
					//fl.trace("fcountNum = "+fcountNum+" t="+t);
				}
				
			}
			
		}
	}
	return collectallMotionsXML;
}
function exportMotions() {
	var doc = fl.getDocumentDOM(); 
	var collectallMotionsXML="All Motions";
	var animCounts=0;
	for(var i=0; i<doc.timelines.length;i++){
		var my_tl = doc.timelines[i]; 
		var countNum = my_tl.frameCount;
		fl.trace(countNum); 
		for(var l=0;l<my_tl.layerCount;l++){
			var my_ly =my_tl.layers[l];
			var fcountNum =my_ly.frameCount;
			//fl.trace(fcountNum); 
			//fl.trace(my_ly.frames.length); 
			//fl.trace("Layer-Animation = "+my_ly.animationType); 
			if(my_ly.animationType=="none"){// this still can be classic tween
				for(var t=0;t<fcountNum;t++){
					var my_tw=my_ly.frames[t];
					if(my_tw.tweenType=="motion"){
						if(my_tw.duration>1){
							fl.trace("tween-type = "+my_tw.tweenType);
							//if(!doc.timelines[i].layers[l].frames[t].isEmpty){ 
							if(my_tw.elements.length==1){
									var waslocked=my_ly.locked;
									my_ly.locked = false;
									my_tl.setSelectedLayers(l);
									
									my_tl.setSelectedFrames(t, t+my_tw.duration); // 'f' is the frame number, start & end
									var file = fl.configURI + 'Javascript/MotionXML.jsfl';
									
									var newMotion=fl.runScript(file, 'copyMotion');
									collectallMotionsXML+="#$TWEEN$#CLASSICTWEEN TimeLine "+my_tl.name+" LayerIdx "+l+" frame "+t+ " duration "+my_tw.duration+"\n";
									//fl.trace(newMotion);
									collectallMotionsXML+=newMotion+"\n";
									animCounts++;
									my_ly.locked = waslocked;
							}
						}
					}
					if(my_tw.tweenType=="shape"){
						fl.trace("found shape");
					}
					else{
						// no tween found						
					}
						
					t+=my_tw.duration-1;
				}
				
			}
			if(my_ly.animationType=="motion object"){
				
				for(var t=0;t<fcountNum;t++){
					var my_tw=my_ly.frames[t];
					//fl.trace(my_tw.duration);
					//fl.trace(my_tw.hasMotionPath());
					if(my_tw.isMotionObject()){
						fl.trace("fcountNum = "+fcountNum+" t="+t);		
						collectallMotionsXML+="#$TWEEN$#MOTIONOBJECT TimeLine "+my_tl.name+" LayerIdx "+l+" frame "+t+ " duration "+my_tw.duration+"\n";
						//fl.trace(newMotion);
						motionXML=doc.timelines[i].layers[l].frames[t].getMotionObjectXML();
						//fl.trace("nodes num: " + motionXML.PropertyContainer.length());
						collectallMotionsXML+=motionXML+"\n";					
						//fl.trace("node 2 name: " + motionXML.;
						animCounts++;
									
					}
					t+=my_tw.duration-1;
					//fl.trace("fcountNum = "+fcountNum+" t="+t);
				}
				
			}
			
		}
	}
	return collectallMotionsXML;
}

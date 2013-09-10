if (window.$ === undefined) {
document.write('<link rel="stylesheet" href="/gui/themes/custom-theme/jquery-ui-1.10.3.custom.css" />');

document.write('<script src="/deps/jquery.js"><\/' + 'script>');
document.write('<script src="/deps/jquery-ui.js"><\/' + 'script>');
document.write('<script src="/deps/jquery.layout.min.js"><\/' + 'script>');}

(function () {

//define LAYOUT as an object
LAYOUT = this.LAYOUT = {};

//initialization attributes. I know it is useless in javascript. It's just to keep a track of my variables
/*
LAYOUT.id = '';
LAYOUT.position = '';
LAYOUT.html = ''; 
LAYOUT.north = {};
LAYOUT.south = {};
LAYOUT.west = {};
LAYOUT.est = {};
LAYOUT.center = {};
LAYOUT.pane = [];
LAYOUT.pane.size;
LAYOUT.bufferChild = [];*/

//Initialisation layout
LAYOUT.init = function(id,position){
	LAYOUT.id=id;
	LAYOUT.position=position;
	LAYOUT.parent= LAYOUT;
	LAYOUT.child = LAYOUT;
	LAYOUT.jqueryObject = $([]);
	LAYOUT.html = '';  
	LAYOUT.north = {};
	LAYOUT.south = {};
	LAYOUT.west = {};
	LAYOUT.est = {};
	LAYOUT.center = {};
	LAYOUT.pane = [0,0,0,0,0];
	LAYOUT.pane.size = 0;
	return LAYOUT;}
	
LAYOUT.reset = function(){
	LAYOUT.id = '';
	LAYOUT.position = 1;
	LAYOUT.html = ''; 
	LAYOUT.parent= $([]);
	LAYOUT.jqueryObject = $([]);
	LAYOUT.north = {};
	LAYOUT.south = {};
	LAYOUT.west = {};
	LAYOUT.est = {};
	LAYOUT.center = {};
	LAYOUT.pane = [0,0,0,0,0];
	LAYOUT.pane.size = 0;
	LAYOUT.bufferChild = [];
}
	
//check layout compatibility, if there are enough components to create a layout
LAYOUT.checkIn = function() {
	var checkNorth = jQuery.isEmptyObject(LAYOUT.north) ? false : true;
	if(checkNorth&&! LAYOUT.pane[0]){LAYOUT.pane[0]="1";LAYOUT.pane.size++;}
	var checkSouth = jQuery.isEmptyObject(LAYOUT.south) ? false : true;
	if(checkSouth&&!LAYOUT.pane[1]){LAYOUT.pane[1]="1";LAYOUT.pane.size++;}
	var checkEst = jQuery.isEmptyObject(LAYOUT.est) ? false : true;
	if(checkEst&&!LAYOUT.pane[2]){LAYOUT.pane[2]="1";LAYOUT.pane.size++;}
	var checkWest = jQuery.isEmptyObject(LAYOUT.west) ? false : true;
	if(checkWest&&!LAYOUT.pane[3]){LAYOUT.pane[3]="1";LAYOUT.pane.size++;}
	var checkCenter = jQuery.isEmptyObject(LAYOUT.center) ? false : true;
	if(checkCenter&&!LAYOUT.pane[4]){LAYOUT.pane[4]="1";LAYOUT.pane.size++;}
	var checkEmpty = jQuery.isEmptyObject(LAYOUT.north)&&jQuery.isEmptyObject(LAYOUT.south)&&jQuery.isEmptyObject(LAYOUT.est)&&jQuery.isEmptyObject(LAYOUT.west)&&jQuery.isEmptyObject(LAYOUT.center) ? true : false;
	if((checkEmpty)||(LAYOUT.pane.size<2)){return false;}
	else{return true;}
}

LAYOUT.checkOut = function(){
	var txt = "Layout{"+LAYOUT.id +"} has "+LAYOUT.pane.size+" panes. North:"+LAYOUT.pane[0]+" South:"+LAYOUT.pane[1]+" Est:"+LAYOUT.pane[2]+" West:"+LAYOUT.pane[3]+" Center:"+LAYOUT.pane[4];
	return txt;
}

LAYOUT.paneInfo = function(nb){
	if((!nb)&&((LAYOUT.pane[nb]))){return {name:'north',link:LAYOUT.north};}
	if((nb==1)&&(LAYOUT.pane[nb])){return {name:"south",link:LAYOUT.south};}
	if((nb==2)&&(LAYOUT.pane[nb])){return {name:'est',link:LAYOUT.est};}
	if((nb==3)&&(LAYOUT.pane[nb])){return {name:'west',link:LAYOUT.west};}
	if((nb==4)&&(LAYOUT.pane[nb])){return {name:'center',link:LAYOUT.center};}
	else{return false;}
}

//Creation Layout
LAYOUT.create = function(){
	if(LAYOUT.checkIn()){ 
		console.LOG("create "+LAYOUT.checkOut());
		var layoutBuffer={}; 
		layoutBuffer['togglerLength_open'] =0;
		for(var i=0;i<LAYOUT.pane.length;i++){
			if(LAYOUT.paneInfo(i)){
				console.debug(this.id);
				LAYOUT.html += '<div id="'+LAYOUT.id+'-'+LAYOUT.paneInfo(i).name+'" class="ui-layout-'+ LAYOUT.paneInfo(i).name +'"></div>';
				layoutBuffer[LAYOUT.paneInfo(i).name] =LAYOUT.paneInfo(i).link;}}
		console.debug(LAYOUT.html);
		LAYOUT.jqueryObject=$(LAYOUT.html);
		if(LAYOUT.position==1){LAYOUT.parent = $('body');}
		LAYOUT.parent.append(LAYOUT.jqueryObject);
		LAYOUT.parent.layout(layoutBuffer);
		}
	else{console.error("Can't create "+LAYOUT.checkOut());}}

LAYOUT.randomColor= function(){
	for(var i=0;i<LAYOUT.pane.length;i++){
		if(LAYOUT.paneInfo(i)){
		var color ='#'+Math.floor(Math.random()*16777215).toString(16); //paul irish 
		$('#'+LAYOUT.id+'-'+LAYOUT.paneInfo(i).name).css('background',color);}
		}}

	
//$('body').append("<h2>"+LAYOUT.id+"</h2>");
LAYOUT.cutV = function(id){
	//this.child.parent=$('#mainLayout-north');
	/*var tmp = this; 
	console.log(this.id);
	this.child.init(id,2);
	this.child.parent = tmp; 
	console.log(tmp.id);
	this.child.center = { size:50 };
	this.child.north = {size:50 };
	this.child.parent=$('#mainLayout-north');
	this.child.create();
	this.child.randomColor();
	return this.child;*/
	var tmp = new LAYOUT.init(id,2);
	tmp.center = { size:50 };
	tmp.north = {size:50 };
	console.debug(tmp.id);
	console.debug(LAYOUT.id);
	//tmp.parent=$('#'+this.id+'-north');
	tmp.create();
	tmp.randomColor();
}

LAYOUT.cutH = function(){}

}).call(this);

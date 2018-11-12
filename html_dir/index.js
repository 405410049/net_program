var speed=8;
var timer;	
var mydiv=document.getElementById("mydiv");
var pic2=document.getElementById("pic2");
pic2.innerHTML=document.getElementById("pic1").innerHTML;

function picMarquee(){
	if(pic2.offsetWidth - mydiv.scrollLeft <= 0)
	{
		mydiv.scrollLeft = 0;
	}
	else
	{
		mydiv.scrollLeft++;
	}
}
timer=setInterval(picMarquee,speed);

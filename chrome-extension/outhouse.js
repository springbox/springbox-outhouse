$(function(){
	$.ajax({
		url: "https://springbox-outhouse.herokuapp.com/",
		method: "GET",
		dataType: "json",
		crossDomain: true,
		success: function(data){
			console.log(data);
			if(data.isOpen == "false")
				$('h1').html('closed');
			else{
				$('h1').html('open');
			}
		}
	});
});
$(document).ready(function(){
	$("select[name='genre']").change(changeResponse);
});

function changeResponse(){
	var genreList = $("input[name='genreListVal']").val();
	if(genreList.indexOf($("select[name='genre'] option:selected").text()) < 0){
		if(genreList!="")
			var newText = genreList+" "+$("select[name='genre'] option:selected").text();
		else
			var newText = $("select[name='genre'] option:selected").text();
		$("input[name='genreListVal']").val(newText);
	}
};
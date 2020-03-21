<!DOCTYPE html>
<html>

	<head>
		<meta charset="ISO-8859-1">
    	<title> Create Account </title>
	</head>

	<body>
		<h1> Create New Account </h1>
		<p> Please enter proposed name and password. </p>
		
		<form action = "CreateServlet" method = "post">
			User Name : <input type = "text" name = "username" value = ""> <br>
			Password : <input type = "text" name = "password" value = "">
			<input type = "submit" value = "Login">
		</form>
		
	</body>

</html>

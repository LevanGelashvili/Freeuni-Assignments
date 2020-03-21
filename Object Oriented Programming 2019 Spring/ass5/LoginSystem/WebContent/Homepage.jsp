<!DOCTYPE html>
<html>

	<head>
		<meta charset="ISO-8859-1">
    	<title> Welcome </title>
	</head>

	<body>
		<h1> Welcome to Homework 5 </h1>
		<p> Please log in. </p>
		
		<form action = "LoginServlet" method = "post">
			User Name : <input type = "text" name = "username" value = ""> <br>
			Password : <input type = "text" name = "password" value = "">
			<input type = "submit" value = "Login">
		</form>
		
		<a href = "Create New Account.jsp" > Create Account </a>
		
	</body>

</html>

<!DOCTYPE html>
<%@page import="Logic.Constants"%>
<%@page import="Logic.Product"%>
<%@page import="Logic.ProductManager"%>
<html>

	<%
		ProductManager manager = (ProductManager) request.getServletContext().getAttribute(Constants.MANAGER_ATTR);
		Product cur = manager.getProduct(request.getParameter(Constants.ID_ATTR));
		String image = Constants.PATH + cur.getImage();
	%>

	<head>
	
		<meta charset="ISO-8859-1">
    	<title> <%= cur.getName() %> </title>
    	
	</head>

	<body>
	
		<h1> <%= cur.getName() %> </h1>
		<img alt = "not found" src = " <%= image %> ">
		<p> <%= cur.getPrice() + " $" %></p>
		
		<form action = "CartServlet" method = "post">
			<input name = <%= Constants.ID_ATTR %> type = hidden value = <%= cur.getId() %>>
			<input type = "submit" value = "Add To Cart">
		</form>
		
	</body>

</html>

<!DOCTYPE html>

<%@page import="java.math.RoundingMode"%>
<%@page import="java.math.BigDecimal"%>
<%@page import="java.util.ArrayList"%>
<%@page import="javafx.util.Pair"%>
<%@page import="java.util.List"%>
<%@page import="java.util.Map.Entry"%>
<%@page import="java.util.Map"%>
<%@page import="java.util.Iterator"%>
<%@page import="Logic.Product"%>
<%@page import="Logic.ShoppingCart"%>
<%@page import="Logic.Constants"%>

<html>

	<head>
		<meta charset="ISO-8859-1">
    	<title> Shopping Cart </title>
	</head>

	<body>
	
		<h1> Shopping Cart </h1>
		
		
		<form action = "CartServlet" method = "post"> <ul>
			<%
				ShoppingCart cart = (ShoppingCart) session.getAttribute(Constants.CART_ATTR);
				Iterator<Map.Entry<Product, Integer>> it = cart.getProducts();

				while (it.hasNext()) {
					Entry<Product, Integer> cur = it.next();
					
					out.print("<li> <input type = 'number' value = " + cur.getValue() 
					+ " name = " + cur.getKey().getId() + "> " + cur.getKey().getName() 
						+ " " + cur.getKey().getPrice() +  "</li>");
				}
			%>
		</ul>
		
		<p> Total : $ <%=  new BigDecimal(cart.getTotalPrice()).setScale(2, RoundingMode.HALF_UP).doubleValue() %> </p>
		
		<input type = "submit" value = "Update Cart"> <br> <br>
		
		</form>
		
		<a href = <%= Constants.HOMEPAGE %>> Continue Shopping </a>
		
	</body>

</html>

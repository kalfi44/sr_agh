package pl.edu.agh.sr;

import java.io.BufferedReader;
import java.io.Console;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Scanner;

public class Main {
	
	public static void main(String[] args) throws Exception{
	
		System.setProperty("java.net.preferIPv4Stack","true");
        DistributedMap distributedMap = new DistributedMap("myCluster");
	
        System.out.println("Enter your instruction: ");
   
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in));
        
        System.out.println(bufferedReader);
        boolean work = true;
        while(work) {
        	String cmd = bufferedReader.readLine();
        	if(cmd!=null) {
        		String arr[] = cmd.split(" ");
	           	switch (arr[0]) {
	    			case "put":
	   					if(arr[1] != null && arr[2] != null) {
	   						distributedMap.put(arr[1], Integer.valueOf(arr[2]));
	   					}
	    				break;
	    			case "remove":
	    				if(arr[1] != null) {
	    					System.out.println(distributedMap.remove(arr[1]));
	    				}
	    				break;
	    			case "get":
	    				if(arr[1] != null) {
	    					System.out.println(distributedMap.get(arr[1]));
	    				}
	    				break;
	    			case "hasKey":
	    				if(arr[1] != null) {
	    					System.out.println(distributedMap.containsKey(arr[1]));
	    				}
	    				break;
	    			case "quit":
	    				work = false;
	    				break;
	    			default:
	    				System.out.println("Wrong command. "
	    						+ "Correct format:\n"
	    						+ "put {key} {value}\n"
	    						+ "remove|get|hasKey {key}\n"
	    						+ "quit\n"
	    						);
	    				break;
	    			}
	        	}
        	}
		}
	
	}


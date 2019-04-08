package pl.edu.agh.sr;


import java.io.Serializable;
import java.net.InetAddress;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import org.jgroups.*;
import org.jgroups.protocols.*;
import org.jgroups.protocols.pbcast.*;
import org.jgroups.stack.ProtocolStack;

public class DistributedMap implements SimpleStringMap{

	private HashMap<String, Integer> distributedMap = new HashMap<>();
	private JChannel channel;
	private String clusterName;
	
	public DistributedMap(String _clusterName) throws Exception {
		clusterName = _clusterName;
		
		channel = new JChannel(false);
		
		ProtocolStack stack = new ProtocolStack();
        channel.setProtocolStack(stack);
        try{
        	stack.addProtocol(new UDP().setValue("mcast_group_addr", InetAddress.getByName("230.100.200.1")))
            .addProtocol(new PING())
            .addProtocol(new MERGE3())
            .addProtocol(new FD_SOCK())
            .addProtocol(new FD_ALL().setValue("timeout", 12000).setValue("interval", 3000))
            .addProtocol(new VERIFY_SUSPECT())
            .addProtocol(new BARRIER())
            .addProtocol(new NAKACK2())
            .addProtocol(new UNICAST3())
            .addProtocol(new STABLE())
            .addProtocol(new GMS())
            .addProtocol(new UFC())
            .addProtocol(new MFC())
            .addProtocol(new FRAG2())
            .addProtocol(new STATE());
            //.addProtocol(new SEQUENCER())
            //.addProtocol(new FLUSH());

        	stack.init();
    	
    	channel.setReceiver(new pl.edu.agh.sr.MyReceiver(this, channel));
    	channel.connect(clusterName);
    	//Thread.sleep(5000);
    	channel.getState(null, 300000);
        }catch(Exception e) {
        	System.out.println("Error intinializing stack");
        	e.printStackTrace();
        }
 
	}
	
	@Override
	public boolean containsKey(String key) {
		return distributedMap.containsKey(key);
	}

	@Override
	public Integer get(String key) {
		int val = distributedMap.get(key);
		return val;
	}

	
	@Override
	public void put(String key, Integer value) {
		try {
			String msg= "PUT " + key +  " " + Integer.toString(value);
			sendMsg(msg);
		}catch(Exception e) {
			e.printStackTrace();
		}
		distributedMap.put(key,value);
		//for (Entry<String, Integer> entry: distributedMap.entrySet()) {
		//	System.out.println(entry.getKey() + " " + entry.getValue());
		//}
		return;
	}

	@Override
	public Integer remove(String key) {
		try {
			String msg= "REMOVE " + key;
			sendMsg(msg);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return distributedMap.remove(key);
	}

	public void setState(Map<? extends String, ? extends Integer> newState) {
		distributedMap.clear();
		distributedMap.putAll(newState);

	}
	
	public HashMap<String, Integer> getMap() {
		return distributedMap;
	}
	
	public void sendMsg(String textMsg) throws Exception{
		Message msg = new Message(null, null, textMsg);
		channel.send(msg);
	}

	
}

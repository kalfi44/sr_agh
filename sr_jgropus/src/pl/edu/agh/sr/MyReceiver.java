package pl.edu.agh.sr;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import org.jgroups.*;
import org.jgroups.util.Util;


public class MyReceiver extends ReceiverAdapter{
	
	private final DistributedMap distributedMap;
	private final JChannel channel;
	

	public MyReceiver(DistributedMap distributedMap, JChannel channel) {
		this.distributedMap = distributedMap;
		this.channel = channel;
	}
		@Override	
		public void viewAccepted(View new_view) {
			System.out.println("received view " + new_view);
			handleView(channel, new_view);
		}
		
		@Override	
		public void getState(OutputStream output) throws Exception {
			synchronized (distributedMap) {
				Util.objectToStream(distributedMap.getMap(), new DataOutputStream(output));
			}
		}
			
		@Override
		public void setState(InputStream inputStream) throws Exception {
			Map<String, Integer> newState;
			newState=(HashMap<String, Integer>) Util.objectFromStream(new DataInputStream(inputStream));
			synchronized (distributedMap) {
				distributedMap.setState(newState);
			}
		}
		
		@Override
		public void receive(Message msg) {
			String textMsg = (String) msg.getObject();
			System.out.println("Msg: " + textMsg + " (source: " + msg.getSrc() + ")");
			
			String [] arr = textMsg.split(" ");
			String instruction = arr[0];
			
			switch(instruction) {
				case "PUT":
					distributedMap.getMap().put(arr[1],Integer.valueOf(arr[2]));
					break;
				case "REMOVE":
					distributedMap.getMap().remove(arr[1]);
					break;
				}
			
		}
	

		
		private static void handleView(JChannel ch, View new_view) {
			if(new_view instanceof MergeView) {
				ViewHandler handler = new ViewHandler(ch, (MergeView)new_view);
				handler.start();
			}
		}
		
		private static class ViewHandler extends Thread {
			JChannel ch;
			MergeView view;
			
			private ViewHandler(JChannel ch, MergeView view) {
				this.ch = ch;
				this.view = view;
			}
			
			public void run() {
				Vector<View> subgroups=(Vector<View>) view.getSubgroups();
		        View tmp_view=subgroups.firstElement(); // picks the first
		        Address local_addr=ch.getAddress();
		        if(!tmp_view.getMembers().contains(local_addr)) {
		            System.out.println("Not member of the new primary partition ("
		                               + tmp_view + "), will re-acquire the state");
		            try {
		                ch.getState(null, 30000);
		            }
		            catch(Exception ex) {
		            }
		        }
		        else {
		            System.out.println("(?)Member of the new primary partition ("
		                               + tmp_view + "), will do nothing");
		        }
			}
		}

}

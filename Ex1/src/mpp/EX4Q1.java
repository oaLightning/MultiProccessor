package mpp;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;

public class EX4Q1 {
	static interface Lock {
		void lock();
		void unlock();
	}
	
	static class QNode {
		volatile boolean locked = true;
		public QNode(boolean value) {
			locked = value;
		}
	}
	
	static class CLHLock implements Lock {
		AtomicReference<QNode> tail;
		ThreadLocal<QNode> myPred;
		ThreadLocal<QNode> myNode;
		
		public CLHLock() {
			tail = new AtomicReference<QNode>(new QNode(false));
			myNode = new ThreadLocal<QNode>() {
				protected QNode initialValue() {
					return new QNode(true);
				}
			};
			myPred = new ThreadLocal<QNode>() {
				protected QNode initialValue() {
					return null;
				}
			};
		}
		
		public void lock() {
			QNode qnode = myNode.get();
			qnode.locked = true;
			QNode pred = tail.getAndSet(qnode);
			myPred.set(pred);
			while (pred.locked) {}
		}	
		
		public void unlock() {
			QNode qnode = myNode.get();
			qnode.locked = false;
			myNode.set(myPred.get());
		}
	}
	
	public interface Handler {
		void onEmpty();
	}
	
	public class Rooms {
		static final int NONE = -1;
		private CLHLock lock = new CLHLock();
		Handler[] exitHandlers;
		AtomicInteger activeRoom = new AtomicInteger(NONE);
		AtomicInteger peopleInRoom = new AtomicInteger(0);
		int numberOfRooms;
		
		public Rooms(int m) {
			exitHandlers = new Handler[m];
		}
		
		public void enter(int roomNumber) {
			lock.lock();
			while ((activeRoom.get() != NONE) && (activeRoom.get() != roomNumber));
			if (activeRoom.get() == roomNumber) {
				int me = peopleInRoom.getAndIncrement();
				if (me == 0) {
					while (activeRoom.get() != NONE);
				}
			}
			if (activeRoom.get() == NONE) {
				activeRoom.set(roomNumber);
				peopleInRoom.set(1);
			}
			lock.unlock();
		}
		
		public boolean exit() {
			int room = activeRoom.get();
			Handler currentHandler = exitHandlers[room];
			if (peopleInRoom.decrementAndGet() == 0) {
				if (null != currentHandler) {
					currentHandler.onEmpty();
				}
				activeRoom.set(NONE);
				return true;
			}
			return false;
		}
		
		public void setExitHandler(int room , Handler handler) {
			exitHandlers[room] = handler;
		}
	}
}

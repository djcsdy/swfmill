class org.swfmill.Foo extends MovieClip {
	var text:String;
	var someValue:String = "foo";
	
	function Foo() {
			Main.debug("inheritance: OK");
		/*
			var png = _root.attachMovie("png","png",_root.getNextHighestDepth());
			Main.debug("png size: "+png._width+"x"+png._height);
		*/
		}

	function onRelease() {
			Main.debug("Release on imported, registerClass'd object: "+this.text );
		}
}

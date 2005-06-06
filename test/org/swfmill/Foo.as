class org.swfmill.Foo extends MovieClip {
	var text:String;
	var someValue:String = "foo";
	
	function Foo() {
			Main.debug("inheritance: OK");
		}

	function onRelease() {
			Main.debug("Release on imported, registerClass'd object: "+this.text );
		}
}

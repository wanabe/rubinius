describe :float_modulo, :shared => true do
  it "returns self modulo other" do
    6543.21.send(@method, 137).should be_close(104.21, TOLERANCE)
    5667.19.send(@method, bignum_value).should be_close(5667.19, TOLERANCE)
    6543.21.send(@method, 137.24).should be_close(92.9299999999996, TOLERANCE)

    6543.21.send(@method, 137).should be_close(6543.21.%(137), TOLERANCE)
    5667.19.send(@method, bignum_value).should be_close(5667.19.%(0xffffffff), TOLERANCE)
    6543.21.send(@method, 137.24).should be_close(6543.21.%(137.24), TOLERANCE)

    -1.0.send(@method, 1).should == 0
  end

  ruby_version_is ""..."1.9" do
    it "does NOT raise ZeroDivisionError if other is zero" do
      1.0.send(@method, 0).to_s.should == 'NaN'
      1.0.send(@method, 0.0).to_s.should == 'NaN'
    end
  end

  ruby_version_is "1.9" do
    it "raises a ZeroDivisionError if other is zero" do
      lambda { 1.0.send(@method, 0) }.should raise_error(ZeroDivisionError)
      lambda { 1.0.send(@method, 0.0) }.should raise_error(ZeroDivisionError)
    end
  end
end

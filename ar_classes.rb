class ARCreateEntryReturn
  attr_accessor :statusList, :entryId
  def initialize(statusList, entryId)
    @statusList, @entryId = statusList, entryId
  end
end
class ARSetEntryReturn
  attr_accessor :statusList
  def initialize(statusList)
    @statusList = statusList
  end
end
class ARDeleteEntryReturn
  attr_accessor :statusList
  def initialize(statusList)
    @statusList = statusList
  end
end
class ARMergeEntryReturn
  attr_accessor :statusList, :entryId
  def initialize(statusList, entryId)
    @statusList, @entryId = statusList, entryId
  end
end
class ARXMLCreateEntryReturn
  attr_accessor :statusList, :outputDoc
  def initialize(statusList, outputDoc)
    @statusList, @outputDoc = statusList, outputDoc
  end
end
class ARXMLSetEntryReturn
  def initialize(xmlsetEntryreturn)
    
  end
end
class ARXMLDeleteEntryReturn
  attr_accessor :statusList
  def initialize(statusList)
    @statusList = statusList
  end
end
class ARBulkEntryReturnList < Array
  alias numItems length
end
  
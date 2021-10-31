require 'rack'

def error
  open('errors.log', 'a+') do |f|
    f.puts "#{Time.now} missing OTA request"
  end
  puts "\e[31mMISSING OTA ...\e[0m"
end

def track
  return unless $last
  puts "Last OTA request: #{$last} #{Time.now - $last}"
  error if Time.now - $last > 45
  sleep 5
rescue => e
  puts e.message
  puts e.backtrace
  sleep 5
end

Thread.new { loop(&method(:track)) }

# BIN_PATH = '/Users/sven/Development/esp32/esp-idf/examples/system/ota/advanced_https_ota/build/advanced_https_ota.bin'
BIN_PATH = 'build/kiste.bin'

class OTA
  def call(env)
    puts "#{env['REQUEST_METHOD']} #{env['PATH_INFO']} ..."
    case env['PATH_INFO']
    when '/kiste/build/kiste.bin'
      $last = Time.now
      [200, { 'Content-Type' => 'application/octet-stream' }, [File.read(BIN_PATH)]]
    else
      puts "Unknown path: #{env['PATH_INFO']}";
      [404, {'Content-Type' => 'text/html'}, ['not found']]
    end
  end
end

run OTA.new
